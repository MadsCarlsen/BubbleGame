import pyautogui as gui
import time

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
        time.sleep(1)

dim_x = [838,1304]
dim_y = [430,939]


