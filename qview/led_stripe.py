led_stripe_array = []

def led_stripe_init(qview_led_stripe):

    global led_stripe_array

    led_stripe_array = [
        # Creates Led Stripe
        qview_led_stripe.canvas.create_rectangle(185, 5, 205, 20, outline = "black", fill = '#000000', width = 1),
        qview_led_stripe.canvas.create_rectangle(215, 5, 235, 20, outline = "black", fill = '#000000', width = 1),
        qview_led_stripe.canvas.create_rectangle(245, 5, 265, 20, outline = "black", fill = '#000000', width = 1),
        qview_led_stripe.canvas.create_rectangle(275, 5, 295, 20, outline = "black", fill = '#000000', width = 1),
        qview_led_stripe.canvas.create_rectangle(305, 5, 325, 20, outline = "black", fill = '#000000', width = 1),
        qview_led_stripe.canvas.create_rectangle(335, 5, 355, 20, outline = "black", fill = '#000000', width = 1),
        qview_led_stripe.canvas.create_rectangle(365, 5, 385, 20, outline = "black", fill = '#000000', width = 1),
        qview_led_stripe.canvas.create_rectangle(395, 5, 415, 20, outline = "black", fill = '#000000', width = 1),
        qview_led_stripe.canvas.create_rectangle(185, 25, 205, 40, outline = "black", fill = '#000000', width = 1),
        qview_led_stripe.canvas.create_rectangle(215, 25, 235, 40, outline = "black", fill = '#000000', width = 1),
        qview_led_stripe.canvas.create_rectangle(245, 25, 265, 40, outline = "black", fill = '#000000', width = 1),
        qview_led_stripe.canvas.create_rectangle(275, 25, 295, 40, outline = "black", fill = '#000000', width = 1),
        qview_led_stripe.canvas.create_rectangle(305, 25, 325, 40, outline = "black", fill = '#000000', width = 1),
        qview_led_stripe.canvas.create_rectangle(335, 25, 355, 40, outline = "black", fill = '#000000', width = 1),
        qview_led_stripe.canvas.create_rectangle(365, 25, 385, 40, outline = "black", fill = '#000000', width = 1),
        qview_led_stripe.canvas.create_rectangle(395, 25, 415, 40, outline = "black", fill = '#000000', width = 1),
    ]




def led_stripe_set(qview_led_stripe, num, r, g, b):

    r_string = "{:02x}".format(r)
    g_string = "{:02x}".format(g)
    b_string = "{:02x}".format(b)

    color = '#' + r_string + g_string + b_string

    # Modification to match hardware led color
    if (color == '#7f0000'):
        color = '#ff0000'
    elif (color == '#00ff00'):
        color = '#ff0000'
    elif (color == '#0000ff'):
        color = '#ff0000'
    elif (color == '#7f007f'):
        color = '#c42be2'
    elif (color == '#7f7f00'):
        color = '##ffff00'
    elif (color == '#64c800'):
        color = '#ff6f00'
    elif (color == '#3f3f3f'):
        color = '#ffffff'
    elif (color == '#00c864'):
        color = '#fc2a70'

    qview_led_stripe.canvas.itemconfig(led_stripe_array[num], fill = color)

def led_stripe_set_all(qview_led_stripe, r, g, b):

    r_string = "{:02x}".format(r)
    g_string = "{:02x}".format(g)
    b_string = "{:02x}".format(b)

    color = '#' + r_string + g_string + b_string
    for led in led_stripe_array:
        qview_led_stripe.canvas.itemconfig(led, fill = color)
    
def led_stripe_set_half(qview_led_stripe, is_left, r, g, b):

    r_string = "{:02x}".format(r)
    g_string = "{:02x}".format(g)
    b_string = "{:02x}".format(b)

    color = '#' + r_string + g_string + b_string

    if (is_left == True):
        for led in led_stripe_array[0:8]:
            qview_led_stripe.canvas.itemconfig(led, fill = color)
    else:
        for led in led_stripe_array[8:16]:
            qview_led_stripe.canvas.itemconfig(led, fill = color)