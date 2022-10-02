from ast import Global
import os
from tkinter import *
from tkinter.ttk import * # override the basic Tk widgets with Ttk widgets
from tkinter.simpledialog import *
from struct import pack

from PIL import Image, ImageTk
import math

global qview_base
global image_dict
global canvas_dict

def custom_menu_command():
    command_name = "START AUTO"
    command_function =  start_auto

    return (command_name, command_function)


def custom_qview_init(qview):

    global qview_base
    qview_base = qview

    if (qview_base == None):
        return

    global HOME_DIR
    global image_dict
    global canvas_dict

    global angle
    global robot_pos_x
    global robot_pos_y
    global last_sensor_active
    global action_counter
    global mot_esq
    global mot_dir


    HOME_DIR = os.path.dirname(__file__)
    action_counter = 0
    last_sensor_active = 0
    angle = 0
    robot_pos_x = 300
    robot_pos_y = 200
    mot_esq = 0
    mot_dir = 0

    qview_base._view_canvas.set(1)
    qview_base.canvas.configure(width=600, height=600)

    image_dict = {
        "arena": PhotoImage(file=HOME_DIR + "/img/arena.png"),
        "led_on": PhotoImage(file=HOME_DIR + "/img/led_on.png").subsample(3,3),
        "led_off": PhotoImage(file=HOME_DIR + "/img/led_off.png").subsample(3,3),
        "sumo": PhotoImage(file=HOME_DIR + "/img/raiju.png"),
        "num_0" : PhotoImage(file=HOME_DIR + "/img/num0.png"),
        "num_1" : PhotoImage(file=HOME_DIR + "/img/num1.png"),
        "rc_button" : PhotoImage(file=HOME_DIR + "/img/button_rc.png"),
        "auto_button" : PhotoImage(file=HOME_DIR + "/img/button_auto.png"),
        "calib_button" : PhotoImage(file=HOME_DIR + "/img/button_calib.png")
    }

    canvas_dict = {
        "arena" : qview_base.canvas.create_image(300,  300, image=image_dict["arena"]), 
        "led" : qview_base.canvas.create_image(50,  30, image=image_dict["led_on"]), 
        "sumo" : qview_base.canvas.create_image(robot_pos_x,  robot_pos_y, image=image_dict["sumo"]), 
        "num_0" : qview_base.canvas.create_image(50, 570, image=image_dict["num_0"]), 
        "num_1" : qview_base.canvas.create_image(100, 570, image=image_dict["num_1"]), 
        "rc_button" : qview_base.canvas.create_image(350, 570, image=image_dict["rc_button"]), 
        "auto_button" : qview_base.canvas.create_image(450, 570, image=image_dict["auto_button"]), 
        "calib_button" : qview_base.canvas.create_image(550, 570, image=image_dict["calib_button"]), 
    }

    # Buttons
    qview_base.canvas.tag_bind(canvas_dict["num_0"], "<ButtonPress>",  lambda strategy: change_strategy(0))
    qview_base.canvas.tag_bind(canvas_dict["num_1"], "<ButtonPress>", lambda strategy: change_strategy(1))
    qview_base.canvas.tag_bind(canvas_dict["rc_button"], "<ButtonPress>", start_rc)
    qview_base.canvas.tag_bind(canvas_dict["auto_button"], "<ButtonPress>", start_auto)
    qview_base.canvas.tag_bind(canvas_dict["calib_button"], "<ButtonPress>", start_calib)


def custom_user_00_packet(packet):
    data = qview_base.qunpack("xxTxbxb", packet)        
    log = data[1]     
    global mot_esq
    global mot_dir

    qview_base.print_text("Timestamp = %d; Log = %d; Argument = %d"%(data[0], log, data[2]))

    if (log == 0):
        led_stat = data[2]
        if (led_stat == 1):
            qview_base.canvas.itemconfig(canvas_dict["led"], image=image_dict["led_on"])
        else:
            qview_base.canvas.itemconfig(canvas_dict["led"], image=image_dict["led_off"])
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
        qview_base.canvas.itemconfig(canvas_dict["sumo"], image=tk_sumo_rotated)
        qview_base.canvas.move(canvas_dict["sumo"], vel_x, vel_y)

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

