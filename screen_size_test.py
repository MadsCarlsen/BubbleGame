from PIL import Image, ImageGrab


dim_x = [838,1304]
dim_y = [430,939]
im1 = ImageGrab.grab(bbox=(dim_x[0], dim_y[0], dim_x[1], dim_y[1]))
im1.show()
