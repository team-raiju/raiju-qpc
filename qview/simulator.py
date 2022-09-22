
# import qview as QView
# from tkinter import *

class SIMULATOR:
    def __init__(self):

        from playsound import playsound
        
        self.beep_sound = HOME_DIR + "/sound/beep.mp3"

        self.play = playsound


        self.x_off = 0
        self.y_off = 0
        self.count = 0

        # add commands to the Custom menu...
        QView.custom_menu.add_command(label="Custom command",
                                      command=self.cust_command)

        # configure the custom QView.canvas...
        QView.show_canvas() # make the canvas visible
        QView.canvas.configure(width=400, height=260)


        self.led_on_img = PhotoImage(file=HOME_DIR + "/img/led_on.png").subsample(3,3)
        self.led_off_img = PhotoImage(file=HOME_DIR + "/img/led_off.png").subsample(3,3)

        self._led = (\
            QView.canvas.create_image(200,  100, image=self.led_on_img)
        )


    # on_reset() callback
    def on_reset(self):
        # clear the lists
        return


    # on_run() callback
    def on_run(self):
        glb_filter("QS_USER_00")

        # NOTE: the names of objects for current_obj() must match
        # the QS Object Dictionaries produced by the application.
        current_obj(OBJ_AO, "l_sumo_hsm")


    # example of a custom command
    def cust_command(self):
        command(0, 0)
        

    def cust_pause(self, event):
        post("START_RC_SIG")
        QView.print_text("START_RC_SIG")
        


    # Intercept the QS_USER_00 application-specific trace record.
    def QS_USER_00(self, packet):
        data = qunpack("xxTxBxB", packet)        
        log = data[1]
        
        if (log == 0):
            led_stat = data[2]
            if (led_stat == 1):
                QView.canvas.itemconfig(self._led, image=self.led_on_img)
            else:
                QView.canvas.itemconfig(self._led, image=self.led_off_img)
        
        elif (log == 1):
            self.play(self.beep_sound)    
              
        QView.print_text("Timestamp = %d; Log = %d; Argument = %d"%(data[0], log, data[2]))
        # QView.print_text("%s"%(str(packet)))

#=============================================================================
QView.customize(SIMULATOR()) # set the QView customization
