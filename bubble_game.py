import numpy as np
from PIL import Image, ImageGrab
import matplotlib.pyplot as plt
import pyautogui as gui
import time
import subprocess

def connected_recursion(type, indices, board, connected_mat):
    if board[indices] == type:
        if connected_mat[indices] != type:
            # Set the type to show point has been visited
            connected_mat[indices] = type

            # Check neighbors
            iy, ix = indices
            Ny, Nx = board.shape
            if iy > 0:  # Check above
                connected_recursion(type, (iy-1, ix), board, connected_mat)
            if iy < Ny-1:  # Check below
                connected_recursion(type, (iy+1, ix), board, connected_mat)
            if ix > 0:  # Check to the left
                connected_recursion(type, (iy, ix-1), board, connected_mat)
            if ix < Nx-1:  # Check to the right
                connected_recursion(type, (iy, ix+1), board, connected_mat)
        else:
            # This point was already found - end recursion here
            return
    else:
        # Type does not match - end recursion
        return

def find_connected(board, indices):
    connected_mat = np.zeros_like(board)  # Keeps the indices of the flat list
    target_type = board[indices]
    # Call the recursion algorithm to check nearest neighbors
    connected_recursion(target_type, indices, board, connected_mat)
    return connected_mat

def update_board(board_in):
    # Update the board, moving bubbles down and to the right if needed (NOT in place)
    board = board_in.copy()
    N_col, N_row = board.shape

    zero_cols = np.any(board, axis=0)
    # First move the 0's to the top
    for i,col in enumerate(board.T):
        if zero_cols[i]:
            temp_list = [i for i in col if i != 0]
            board.T[i] = [0]*(N_col - len(temp_list)) + temp_list

    # Then move all completely 0 cols to the most left
    board = np.delete(board, np.logical_not(zero_cols), axis=1)
    board = np.concatenate((np.zeros((N_col, N_row - board.shape[1])), board), axis=1)
    return board

def index_to_coord(index, Nx=12):
    mod = index // Nx
    return mod, index - mod*11

def find_sol_recursive(board_in, click_list_in):
    global sol_click_list
    board = board_in.copy()
    visited_groups = np.zeros_like(board)
    N_col, N_row = board.shape  # 12x11 when done

    # Check if solution is found - This is the recursion exit
    if sol_click_list:
        return

    # If solution is just found - save it and break the recursion
    if np.count_nonzero(board_in) == 0:
        print(board)
        print('Solution found!')
        sol_click_list = click_list_in
        return

    # Main loop over all elements of board
    for i in range(N_col * N_row):
        coord = index_to_coord(i, N_row)

        # Check if 0 or already visited
        if board[coord] == 0 or visited_groups[coord] == 1:
            continue
        else:
            # Check if it is connected
            connect_mat = find_connected(board, coord)
            if np.count_nonzero(connect_mat) == 1:  # This seem a bit unefficient. Should save conncted indices as a list!
                # It is by itself - move along
                continue
            else:
                # It is in a group - pop it, update the board and pass on the recursion
                to_pop = connect_mat != 0
                board[to_pop] = 0
                board = update_board(board)
                new_click_list = click_list_in + [coord]
                find_sol_recursive(board, new_click_list)

    # End of for loop
    return

def find_closest_item(color_array):
    #item = ['pants', 'bag', 'shirt', 'shoe']
    item = [1, 2, 3, 4]  # Corresponds to the value above
    color = np.array([[56, 54, 55], [142, 58, 11], [196, 14, 105], [179, 119, 204]])
    diff = np.sum(np.abs(color_array - color),axis=1)
    return item[np.argmin(diff)]

def find_dominant_color(im, palette_size=50):
    paletted = im.convert('P', palette=Image.ADAPTIVE, colors=palette_size)

    # Find the color that occurs most often
    palette = paletted.getpalette()
    color_counts = sorted(paletted.getcolors(), reverse=True)
    palette_index = color_counts[0][1]
    dominant_color = palette[palette_index * 3:palette_index * 3 + 3]
    return dominant_color

def load_board(dim_x, dim_y):
    # MAKE THIS FUNCTION MORE PRETTY PLEASE
    im_x = dim_x[1] - dim_x[0]
    im_y = dim_y[1] - dim_y[0]
    bubble_x = im_x // 11
    bubble_y = im_y // 12
    im1 = ImageGrab.grab(bbox=(dim_x[0], dim_y[0], dim_x[1], dim_y[1]))

    epsilon = 7
    ting = []

    for row_i in range(12):
        ting_row = []
        for col_i in range(11):
            bubble_i = (row_i, col_i)

            start_x = bubble_i[1] * bubble_x + epsilon
            end_x = start_x + bubble_x - 2 * epsilon
            start_y = bubble_i[0] * bubble_y + epsilon
            end_y = start_y + bubble_y - 2 * epsilon

            im2 = im1.crop((start_x, start_y, end_x, end_y))#.convert('L')
            dominant_color = find_dominant_color(im2)
            ting_row.append(find_closest_item(dominant_color))
        ting.append(ting_row)
    return np.array(ting)

def save_board_to_file(game_board): 
    with open('boarddata.txt', 'w') as file: 
        for val in game_board.flatten():    
            file.write(f'{val} ')

def click_solutions(dim_x, dim_y):
    # First load the solutions
    row_list, col_list = np.loadtxt('solution.txt', unpack=True)
    im_x = dim_x[1] - dim_x[0]
    im_y = dim_y[1] - dim_y[0]
    bubble_x = im_x // 11
    bubble_y = im_y // 12

    for row_i, col_i in zip(row_list, col_list):
        mouse_x = dim_x[0] + col_i*bubble_x + bubble_x/2
        mouse_y = dim_y[0] + row_i*bubble_y + bubble_y/2
        print(mouse_x, mouse_y)
        gui.click(mouse_x, mouse_y)
        time.sleep(0.6)

dim_x = [838,1304]
dim_y = [430,939]


while True: 
    game_board = load_board(dim_x, dim_y)
    save_board_to_file(game_board)
    print('Finding solutions ... ')
    return_code = subprocess.call("./WardrobeChallenge",shell=True)
    print('Starting to click!')
    click_solutions(dim_x, dim_y)
    time.sleep(1)
    


