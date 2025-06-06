import os
import matplotlib.pyplot as plt
from statistics import mean, stdev


def plot_velocities(file_path):
 
    # Lists to store the data from valid lines
    time = []
    linear_velocity1 = []
    linear_velocity2 = []
    angular_velocity1 = []
    angular_velocity2 = []
    pwm_left = []
    pwm_right = []
    
    # Range validation constants
    LINEAR_VEL_MAX = 10.0
    LINEAR_VEL_MIN = -10.0
    LINEAR_VEL_MAX_DELTA = 1.0

    ANGULAR_VEL_MAX = 70.0
    ANGULAR_VEL_MIN = -70.0
    ANGULAR_VEL_MAX_DELTA = 0.8

    PWM_MIN = -100
    PWM_MAX = 100
    PWM_MAX_DELTA = 20

    with open(file_path, 'r') as file:
        current_time = 0

        for line_num, line in enumerate(file, 1):
            current_time += 1
            try:
                fields = line.strip().split(';')

                # 1. Check for the correct number of fields to avoid IndexError
                if len(fields) < 8:
                    print(f"Warning: Skipping line {line_num}. Expected at least 8 fields, but got {len(fields)}.")
                    continue

                # 2. Attempt to convert fields to float. A ValueError will be raised
                lv1 = float(fields[1])
                lv2 = float(fields[2])
                av1 = float(fields[3])
                av2 = float(fields[4])
                pl = float(fields[6])
                pr = float(fields[7])

                # 3. Perform range validation
                if not (LINEAR_VEL_MIN <= lv1 <= LINEAR_VEL_MAX):
                    print(f"Warning: Skipping line {line_num}. Left linear velocity '{lv1}' is out of range ({LINEAR_VEL_MIN}, {LINEAR_VEL_MAX}).")
                    continue
                if not (LINEAR_VEL_MIN <= lv2 <= LINEAR_VEL_MAX):
                    print(f"Warning: Skipping line {line_num}. Right linear velocity '{lv2}' is out of range ({LINEAR_VEL_MIN}, {LINEAR_VEL_MAX}).")
                    continue
                if not (ANGULAR_VEL_MIN <= av1 <= ANGULAR_VEL_MAX):
                    print(f"Warning: Skipping line {line_num}. Left angular velocity '{av1}' is out of range ({ANGULAR_VEL_MIN}, {ANGULAR_VEL_MAX}).")
                    continue
                if not (ANGULAR_VEL_MIN <= av2 <= ANGULAR_VEL_MAX):
                    print(f"Warning: Skipping line {line_num}. Right angular velocity '{av2}' is out of range ({ANGULAR_VEL_MIN}, {ANGULAR_VEL_MAX}).")
                    continue
                if not (PWM_MIN <= pl <= PWM_MAX):
                    print(f"Warning: Skipping line {line_num}. Left PWM value '{pl}' is out of range ({PWM_MIN}, {PWM_MAX}).")
                    continue
                if not (PWM_MIN <= pr <= PWM_MAX):
                    print(f"Warning: Skipping line {line_num}. Right PWM value '{pr}' is out of range ({PWM_MIN}, {PWM_MAX}).")
                    continue

                # 4. Check for angular velocity delta
                if (current_time > 1):

                    if abs(lv1 - linear_velocity1[-1]) > LINEAR_VEL_MAX_DELTA:
                        print(f"Warning: Skipping line {line_num}. Linear velocities '{lv1}' and '{linear_velocity1[-1]}' differ by more than {LINEAR_VEL_MAX_DELTA}.")
                        continue
                    if abs(lv2 - linear_velocity2[-1]) > LINEAR_VEL_MAX_DELTA:
                        print(f"Warning: Skipping line {line_num}. Linear velocities '{lv2}' and '{linear_velocity2[-1]}' differ by more than {LINEAR_VEL_MAX_DELTA}.")
                        continue

                    if abs(av1 - angular_velocity1[-1]) > ANGULAR_VEL_MAX_DELTA:
                        print(f"Warning: Skipping line {line_num}. Angular velocities '{av1}' and '{angular_velocity1[-1]}' differ by more than {ANGULAR_VEL_MAX_DELTA}.")
                        continue
                    if abs(av2 - angular_velocity2[-1]) > ANGULAR_VEL_MAX_DELTA:
                        print(f"Warning: Skipping line {line_num}. Angular velocities '{av2}' and '{angular_velocity2[-1]}' differ by more than {ANGULAR_VEL_MAX_DELTA}.")
                        continue

                    if abs(pl - pwm_left[-1]) > PWM_MAX_DELTA:
                        print(f"Warning: Skipping line {line_num}. PWM values '{pl}' and '{pwm_left[-1]}' differ by more than {PWM_MAX_DELTA}.")
                        continue
                    if abs(pr - pwm_right[-1]) > PWM_MAX_DELTA:
                        print(f"Warning: Skipping line {line_num}. PWM values '{pr}' and '{pwm_right[-1]}' differ by more than {PWM_MAX_DELTA}.")
                        continue


            except ValueError:
                print(f"Warning: Skipping line {line_num}. Contains invalid non-numeric data.")
                continue
            except IndexError:
                print(f"Warning: Skipping line {line_num}. Line is malformed.")
                continue

            # If all checks passed, append the data to the lists
            time.append(current_time)
            linear_velocity1.append(lv1)
            linear_velocity2.append(lv2)
            angular_velocity1.append(av1)
            angular_velocity2.append(av2)
            pwm_left.append(pl)
            pwm_right.append(pr)
            
    print(f"Successfully processed {len(time)} valid data points.")

    # --- Plotting Section (example) ---
    if not time:
        print("No valid data was found to plot.")
        return

    fig, axs = plt.subplots(3, 1, figsize=(12, 18), sharex=True)
    fig.suptitle('Robot Kinematics and PWM Signals', fontsize=16)

    # Plot Linear Velocities
    axs[0].plot(time, linear_velocity1, label='Linear Velocity', color='b')
    axs[0].plot(time, linear_velocity2, label='Target Velocity', color='c', linestyle='--')
    axs[0].set_ylabel('Linear Velocity (m/s)')
    axs[0].legend()
    axs[0].grid(True)

    # Plot Angular Velocities
    axs[1].plot(time, angular_velocity1, label='Angular Velocity', color='r')
    axs[1].plot(time, angular_velocity2, label='Target Velocity', color='m', linestyle='--')
    axs[1].set_ylabel('Angular Velocity (rad/s)')
    axs[1].legend()
    axs[1].grid(True)

    # Plot PWM Signals
    axs[2].plot(time, pwm_left, label='PWM Left', color='g')
    axs[2].plot(time, pwm_right, label='PWM Right', color='orange')
    axs[2].set_ylabel('PWM Value')
    axs[2].set_xlabel('Time (increments)')
    axs[2].legend()
    axs[2].grid(True)
    # Add horizontal lines for PWM limits
    axs[2].axhline(y=PWM_MAX, color='gray', linestyle=':', linewidth=1)
    axs[2].axhline(y=PWM_MIN, color='gray', linestyle=':', linewidth=1)


    plt.tight_layout()
    plt.show()


def plot_integral_and_battery(file_path):

    # Lists to store the data from valid lines
    time = []
    battery = []
    integral_vel = []
    integral_ang_vel = []

    
    # Range validation constants
    INTEGRAL_VEL_MAX = 10000.0
    INTEGRAL_VEL_MIN = -10000.0
    INTEGRAL_VEL_MAX_DELTA = 1000.0

    INTEGRAL_ANGULAR_VEL_MAX = 10000.0
    INTEGRAL_ANGULAR_VEL_MIN = -10000.0
    INTEGRAL_ANGULAR_VEL_MAX_DELTA = 1000.0

    BATTERY_MIN = 35000.0
    BATTERY_MAX = 55000.0
    BATTERY_MAX_DELTA = 2000.0

    with open(file_path, 'r') as file:
        current_time = 0

        for line_num, line in enumerate(file, 1):
            current_time += 1
            try:
                fields = line.strip().split(';')

                # 1. Check for the correct number of fields to avoid IndexError
                if len(fields) < 11:
                    print(f"Warning: Skipping line {line_num}. Expected at least 8 fields, but got {len(fields)}.")
                    continue

                # 2. Attempt to convert fields to float. A ValueError will be raised
                bat = float(fields[8])
                int_1 = float(fields[9])
                int_2 = float(fields[10])

                # 3. Perform range validation
                if not (BATTERY_MIN <= bat <= BATTERY_MAX):
                    print(f"Warning: Skipping line {line_num}. Battery value '{bat}' is out of range ({BATTERY_MIN}, {BATTERY_MAX}).")
                    continue
                if not (INTEGRAL_VEL_MIN <= int_1 <= INTEGRAL_VEL_MAX):
                    print(f"Warning: Skipping line {line_num}. Integral linear velocity '{int_1}' is out of range ({INTEGRAL_VEL_MIN}, {INTEGRAL_VEL_MAX}).")
                    continue
                if not (INTEGRAL_ANGULAR_VEL_MIN <= int_2 <= INTEGRAL_ANGULAR_VEL_MAX):
                    print(f"Warning: Skipping line {line_num}. Integral angular velocity '{int_2}' is out of range ({INTEGRAL_ANGULAR_VEL_MIN}, {INTEGRAL_ANGULAR_VEL_MAX}).")
                    continue


                # 4. Check for angular velocity delta
                if (current_time > 1):

                    if abs(bat - battery[-1]) > BATTERY_MAX_DELTA:
                        print(f"Warning: Skipping line {line_num}. Battery values '{bat}' and '{battery[-1]}' differ by more than {BATTERY_MAX_DELTA}.")
                        continue
                    if abs(int_1 - integral_vel[-1]) > INTEGRAL_VEL_MAX_DELTA:
                        print(f"Warning: Skipping line {line_num}. Integral velocities '{int_1}' and '{integral_vel[-1]}' differ by more than {INTEGRAL_VEL_MAX_DELTA}.")
                        continue
                    if abs(int_2 - integral_ang_vel[-1]) > INTEGRAL_ANGULAR_VEL_MAX_DELTA:
                        print(f"Warning: Skipping line {line_num}. Integral angular velocities '{int_2}' and '{integral_ang_vel[-1]}' differ by more than {INTEGRAL_ANGULAR_VEL_MAX_DELTA}.")
                        continue

            except ValueError:
                print(f"Warning: Skipping line {line_num}. Contains invalid non-numeric data.")
                continue
            except IndexError:
                print(f"Warning: Skipping line {line_num}. Line is malformed.")
                continue

            # If all checks passed, append the data to the lists
            time.append(current_time)
            battery.append(bat)
            integral_vel.append(int_1)
            integral_ang_vel.append(int_2)
            
    print(f"Successfully processed {len(time)} valid data points.")

    # --- Plotting Section (example) ---
    if not time:
        print("No valid data was found to plot.")
        return

    fig, axs = plt.subplots(3, 1, figsize=(12, 18))

    # Plot Battery Voltage
    axs[0].plot(time, battery, label='Battery Voltage', color='g')
    axs[0].set_ylabel('Battery Voltage (mV)')
    axs[0].legend()
    axs[0].grid(True)

    # Plot Linear Velocities
    axs[1].plot(time, integral_vel, label='Integral Linear Velocity', color='b')
    axs[1].set_ylabel('Integral Linear Velocity (m/s)')
    axs[1].legend()
    axs[1].grid(True)

    # Plot Angular Velocities
    axs[2].plot(time, integral_ang_vel, label='Integral Angular Velocity', color='r')
    axs[2].set_ylabel('Angular Velocity (rad/s)')
    axs[2].legend()
    axs[2].grid(True)

    plt.tight_layout()
    plt.show()



if __name__ == "__main__":
    script_dir = os.path.dirname(os.path.abspath(__file__))
    file_name = 'data.txt'
    file_path = os.path.join(script_dir, file_name)
    plot_velocities(file_path)
    plot_integral_and_battery(file_path)