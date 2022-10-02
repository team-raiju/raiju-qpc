from ast import Global
import os
from tkinter import *
from tkinter.ttk import * # override the basic Tk widgets with Ttk widgets
from tkinter.simpledialog import *
from struct import pack

from PIL import Image, ImageTk
import math

global qview_base

def custom_qview_init(qview):

    global qview_base
    qview_base = qview


    if (qview_base == None):
        return

    global HOME_DIR
    global led_on_img
    global led_off_img
    global canvas_img
    global sumo_png
    global sumo_canvas
    global arena_png
    global arena_canvas
    global angle
    global robot_pos_x
    global robot_pos_y
    global last_sensor_active
    global action_counter


    HOME_DIR = os.path.dirname(__file__)

    qview_base._view_canvas.set(1)
    qview_base.canvas.configure(width=600, height=600)
    
    action_counter = 0
    last_sensor_active = 0
    angle = 0
    robot_pos_x = 300
    robot_pos_y = 200

    arena_png = PhotoImage(file=HOME_DIR + "/img/arena.png")
    led_on_img = PhotoImage(file=HOME_DIR + "/img/led_on.png").subsample(3,3)
    led_off_img = PhotoImage(file=HOME_DIR + "/img/led_off.png").subsample(3,3)
    sumo_png = PhotoImage(file=HOME_DIR + "/img/raiju.png")

    arena_canvas = qview_base.canvas.create_image(300,  300, image=arena_png)
    canvas_img = qview_base.canvas.create_image(50,  30, image=led_on_img)
    sumo_canvas = qview_base.canvas.create_image(robot_pos_x,  robot_pos_y, image=sumo_png)

    # Buttons
    global num_0_img
    global num_0_canvas
    global num_1_img
    global num_1_canvas
    global rc_img
    global rc_canvas
    global auto_img
    global auto_canvas
    global calib_img
    global calib_canvas
    global mot_esq
    global mot_dir

    mot_esq = 0
    mot_dir = 0


    num_0_img = PhotoImage(file=HOME_DIR + "/img/num0.png")
    num_1_img = PhotoImage(file=HOME_DIR + "/img/num1.png")
    rc_img = PhotoImage(file=HOME_DIR + "/img/button_rc.png")
    auto_img = PhotoImage(file=HOME_DIR + "/img/button_auto.png")
    calib_img = PhotoImage(file=HOME_DIR + "/img/button_calib.png")

    num_0_canvas = qview_base.canvas.create_image(50, 570, image=num_0_img)
    qview_base.canvas.tag_bind(num_0_canvas, "<ButtonPress>",  lambda strategy: change_strategy(0))

    num_1_canvas = qview_base.canvas.create_image(100, 570, image=num_1_img)
    qview_base.canvas.tag_bind(num_1_canvas, "<ButtonPress>", lambda strategy: change_strategy(1))

    rc_canvas = qview_base.canvas.create_image(350, 570, image=rc_img)
    qview_base.canvas.tag_bind(rc_canvas, "<ButtonPress>", start_rc)

    auto_canvas = qview_base.canvas.create_image(450, 570, image=auto_img)
    qview_base.canvas.tag_bind(auto_canvas, "<ButtonPress>", start_auto)

    calib_canvas = qview_base.canvas.create_image(550, 570, image=calib_img)
    qview_base.canvas.tag_bind(calib_canvas, "<ButtonPress>", start_calib)


def custom_user_00_packet(packet):
    data = qview_base.qunpack("xxTxbxb", packet)        
    log = data[1]     
    global mot_esq
    global mot_dir

    qview_base.print_text("Timestamp = %d; Log = %d; Argument = %d"%(data[0], log, data[2]))

    if (log == 0):
        led_stat = data[2]
        if (led_stat == 1):
            qview_base.canvas.itemconfig(canvas_img, image=led_on_img)
        else:
            qview_base.canvas.itemconfig(canvas_img, image=led_off_img)
    elif (log == 2):
        data = qview_base.qunpack("xxTxbxbxb", packet)    
        mot_esq = data[2]
        mot_dir = data[3]
        qview_base.print_text("MOT_ESQ = %d; MOT_DIR = %d"%(mot_esq, mot_dir))    


def custom_on_poll():
    global action_counter

    action_counter = action_counter + 1
    if (action_counter % 4 == 0):
        global sumo_rotated
        global tk_sumo_rotated
        global angle
        global robot_pos_x
        global robot_pos_y
        global mot_esq
        global mot_dir
        global last_sensor_active

        rotation_vel = (mot_dir - mot_esq) / 2
        angle += (rotation_vel / 10)
        angle = angle % 360

        vel = ((mot_esq + mot_dir) / 2)
        vel_x = int((vel * math.sin(angle * math.pi/180))/10)
        vel_y = int((vel * math.cos(angle * math.pi/180))/10)

        robot_pos_x += vel_x
        robot_pos_y += vel_y

        sumo_rotated = Image.open(HOME_DIR + "/img/raiju.png")
        tk_sumo_rotated = ImageTk.PhotoImage(sumo_rotated.rotate(angle))
        qview_base.canvas.itemconfig(sumo_canvas, image=tk_sumo_rotated)
        qview_base.canvas.move(sumo_canvas, vel_x, vel_y)

        # Check if it is on line
        if (is_over_circle(robot_pos_x, robot_pos_y)):
            line_command()

        # Check mouse:
        sensor_active = is_mouse_direction(robot_pos_x, robot_pos_y, angle)
        if (sensor_active != last_sensor_active):
            sensor_command(sensor_active)

        last_sensor_active = sensor_active

def start_rc(*args):
    qview_base.command(0, 0)

def start_auto(*args):
    qview_base.command(1, 0)

def start_calib(*args):
    qview_base.command(2, 0)

def change_strategy(strategy):
    qview_base.command(3, strategy)

def line_command():
    qview_base.command(4, 0)

def sensor_command(sensor):
    qview_base.command(5, sensor)

def is_over_circle(posx, posy):
    if (((posx - 300 ) ** 2 + (posy - 300 ) ** 2) > 200 ** 2):
        return True
    else:
        return False

def is_mouse_direction(posx, posy, robot_angle):
    mouse_pos_x_abs = qview_base.canvas.winfo_pointerx() - qview_base.canvas.winfo_rootx()
    mouse_pos_y_abs = qview_base.canvas.winfo_pointery() - qview_base.canvas.winfo_rooty()

    mouse_pos_x_rel = mouse_pos_x_abs - posx
    mouse_pos_y_rel = mouse_pos_y_abs - posy

    mouse_angle_rel = math.degrees(math.atan2(mouse_pos_x_rel, mouse_pos_y_rel))
    mouse_robot_distance = math.sqrt(mouse_pos_x_rel ** 2 + mouse_pos_y_rel ** 2)

    # Limit robot angle range
    robot_angle %= 360
    if (robot_angle > 180):
        robot_angle -= 360
    elif (robot_angle < -180):
        robot_angle += 360
    
    anglediff = mouse_angle_rel - robot_angle

    if (mouse_robot_distance < 150):
        if (anglediff > -15 and anglediff < 15):
            return 3
        elif (anglediff > 15 and anglediff < 40):
            return 4
        elif (anglediff > 40 and anglediff < 60):
            return 5
        elif (anglediff > -40 and anglediff < -15):
            return 2
        elif (anglediff > -60 and anglediff < -40):
            return 1
    
    return 0

