from ast import Global
import os
from tkinter import *
from tkinter.ttk import * # override the basic Tk widgets with Ttk widgets
from tkinter.simpledialog import *
from struct import pack

from PIL import Image, ImageTk
import math

from robot import *

global qview_base
global image_dict
global canvas_dict
global sumo_robot

def custom_menu_command():
    command_name = "RESET POSITION"
    command_function =  reset_position

    return (command_name, command_function)


def custom_qview_init(qview):

    global qview_base
    qview_base = qview

    if (qview_base == None):
        return

    global HOME_DIR
    global image_dict, canvas_dict
    global last_sensor_active, action_counter
    global sumo_robot

    HOME_DIR = os.path.dirname(__file__)

    sumo_robot = Robot(300, 100)
    action_counter = 0
    last_sensor_active = 0

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
        "calib_button" : PhotoImage(file=HOME_DIR + "/img/button_calib.png"),
        "idle_button" : PhotoImage(file=HOME_DIR + "/img/button_idle.png"),
        "stop_button" : PhotoImage(file=HOME_DIR + "/img/button_stop.png"),
    }

    canvas_dict = {
        "arena" : qview_base.canvas.create_image(300,  300, image=image_dict["arena"]), 
        "led" : qview_base.canvas.create_image(50,  30, image=image_dict["led_on"]), 
        "sumo" : qview_base.canvas.create_image(sumo_robot.get_position()[0],  sumo_robot.get_position()[1], image=image_dict["sumo"]), 
        "num_0" : qview_base.canvas.create_image(50, 570, image=image_dict["num_0"]), 
        "num_1" : qview_base.canvas.create_image(100, 570, image=image_dict["num_1"]), 
        "rc_button" : qview_base.canvas.create_image(350, 570, image=image_dict["rc_button"]), 
        "auto_button" : qview_base.canvas.create_image(450, 570, image=image_dict["auto_button"]), 
        "calib_button" : qview_base.canvas.create_image(550, 570, image=image_dict["calib_button"]), 
        "idle_button" : qview_base.canvas.create_image(450, 30, image=image_dict["idle_button"]), 
        "stop_button" : qview_base.canvas.create_image(550, 30, image=image_dict["stop_button"]), 
    }

    # Buttons
    qview_base.canvas.tag_bind(canvas_dict["num_0"], "<ButtonPress>",  lambda strategy: change_strategy(0))
    qview_base.canvas.tag_bind(canvas_dict["num_1"], "<ButtonPress>", lambda strategy: change_strategy(1))
    qview_base.canvas.tag_bind(canvas_dict["rc_button"], "<ButtonPress>", start_rc)
    qview_base.canvas.tag_bind(canvas_dict["auto_button"], "<ButtonPress>", start_auto)
    qview_base.canvas.tag_bind(canvas_dict["calib_button"], "<ButtonPress>", start_calib)
    qview_base.canvas.tag_bind(canvas_dict["idle_button"], "<ButtonPress>", idle_command)
    qview_base.canvas.tag_bind(canvas_dict["stop_button"], "<ButtonPress>", stop_command)


def custom_user_00_packet(packet):
    data = qview_base.qunpack("xxTxbxb", packet)        
    log = data[1]     

    qview_base.print_text("Timestamp = %d; Log = %d; Argument = %d"%(data[0], log, data[2]))

    if (log == 0):
        led_stat = data[2]
        if (led_stat == 1):
            qview_base.canvas.itemconfig(canvas_dict["led"], image=image_dict["led_on"])
        else:
            qview_base.canvas.itemconfig(canvas_dict["led"], image=image_dict["led_off"])
    elif (log == 2):
        data = qview_base.qunpack("xxTxbxbxb", packet)    
        sumo_robot.set_motors(data[2], data[3])
        mot_esq, mot_dir = sumo_robot.get_motors()
        qview_base.print_text("MOT_ESQ = %d; MOT_DIR = %d"%(mot_esq, mot_dir))    


def custom_on_poll():
    global action_counter
    action_counter += 1

    if (action_counter % 4 == 0):
        global sumo_rotated_canvas
        global tk_sumo_rotated
        global last_sensor_active

        mot_esq, mot_dir = sumo_robot.get_motors()
        angle = sumo_robot.get_angle()
        
        rotation_vel = (mot_dir - mot_esq) / 2
        angle += (rotation_vel / 10)
        sumo_robot.set_angle(angle % 360)

        vel = ((mot_esq + mot_dir) / 2)
        vel_x = int((vel * math.sin(angle * math.pi/180))/10)
        vel_y = int((vel * math.cos(angle * math.pi/180))/10)

        sumo_robot.offset_position(vel_x, vel_y)

        sumo_rotated_canvas = Image.open(HOME_DIR + "/img/raiju.png")
        tk_sumo_rotated = ImageTk.PhotoImage(sumo_rotated_canvas.rotate(angle))
        qview_base.canvas.itemconfig(canvas_dict["sumo"], image=tk_sumo_rotated)
        qview_base.canvas.move(canvas_dict["sumo"], vel_x, vel_y)

        robot_pos_x, robot_pos_y = sumo_robot.get_position()

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

def idle_command(sensor):
    qview_base.command(6, 0)

def stop_command(sensor):
    qview_base.command(7, 0)

def reset_position():
    global image_dict, canvas_dict
    sumo_robot.set_angle(0)
    sumo_robot.set_position(300, 100)
    canvas_dict["sumo"] = qview_base.canvas.create_image(sumo_robot.get_position()[0],  sumo_robot.get_position()[1], image=image_dict["sumo"])

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

