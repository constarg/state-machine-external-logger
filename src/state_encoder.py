import serial
import sys
import time

# encode checker states.
def encode_checker_signal(sig_checker):
    match sig_checker:
        case "000":
            return "IDLE"
        case "001":
            return "CHECK STATUS"
        case "010":
            return "CHECKPOINT FROM BOTH CORES"
        case "011":
            return "ROLLBACK"
        case "100":
            return "WAKE UP CORES"
        case "101":
            return "WAIT FOR STANDBY"
        case "110":
            return "CHECKPOINT FROM ONE CORE"
        case "111":
            return "ROLLFORWARD"

def get_log_format(timestamp, signal_id, signal_val):
    return "[LOG] [{tstamp}] [{sig_id}] = {value}\n".format(tstamp = timestamp, sig_id = signal_id, value = signal_val)

def get_curr_time_date():
    return "_".join(time.strftime("%H:%M:%S:%d:%m:%y", time.localtime()).split(":"))

def print_encoded_signal(timestamp, signal_id, signal_val):
    print(get_log_format(timestamp, signal_id, signal_val))

def write_log_to_file(timestamp, signal_id, signal_val, log_file): 
    with open(log_file, "a") as log_file:
        log_file.write(get_log_format(timestamp, signal_id, signal_val))
        log_file.write("\n")

# write into a file only the errors (if any).
def write_error_log_to_file(timestamp, signal_id, signal_val, log_error_file):
    with open(log_error_file, "a") as error_file:
        error_file.write(get_log_format(timestamp, signal_id, signal_val))
        error_file.write("\n")

def log_encoded_signals(timestamp, signals, log_file, log_error_file):
    # get the signals and reverse the bit order. Move most significant bit left instead of right.
    checker           = str(signals[0:3])[::-1]  # cheker bits
    core_standbywfe   = str(signals[3:5])[::-1]  # core standbywfe bits.
    core_parity_error = str(signals[5:7])[::-1]  # core core parity error bits.
    watchdog_timer_ex = str(signals[7:8])        # watchdog timer expiration bit.
    forced_standby    = str(signals[8:10])[::-1] # forced standby status bit.
    checkpoint_err_bt = str(signals[10])         # checkpoint error after boot bit.

    # Print each signal.
    print_encoded_signal(timestamp, "Checker", encode_checker_signal(checker))
    print_encoded_signal(timestamp, "Core standbywfe", core_standbywfe)
    print_encoded_signal(timestamp, "Core parity error", core_parity_error)
    print_encoded_signal(timestamp, "Watchdog timer expiration", watchdog_timer_ex)
    print_encoded_signal(timestamp, "Forced standby", forced_standby)
    print_encoded_signal(timestamp, "Checkpoint error after boot", checkpoint_err_bt)

    # save log to a file.
    write_log_to_file(timestamp, "Checker", encode_checker_signal(checker), log_file)
    write_log_to_file(timestamp, "Core standbywfe", core_standbywfe, log_file)
    write_log_to_file(timestamp, "Core parity error", core_parity_error, log_file)
    write_log_to_file(timestamp, "Watchdog timer expiration", watchdog_timer_ex, log_file)
    write_log_to_file(timestamp, "Forced standby", forced_standby, log_file)
    write_log_to_file(timestamp, "Checkpoint error after boot", checkpoint_err_bt, log_file)
    
    # any error has occured?
    if (core_parity_error != "00" or checkpoint_err_bt != "0"):
        # save errors to a new log file.
        write_error_log_to_file(timestamp, "Checker", encode_checker_signal(checker), log_error_file)
        write_error_log_to_file(timestamp, "Core standbywfe", core_standbywfe, log_error_file)
        write_error_log_to_file(timestamp, "Core parity error", core_parity_error, log_error_file)
        write_error_log_to_file(timestamp, "Watchdog timer expiration", watchdog_timer_ex, log_error_file)
        write_error_log_to_file(timestamp, "Forced standby", forced_standby, log_error_file)
        write_error_log_to_file(timestamp, "Checkpoint error after boot", checkpoint_err_bt, log_error_file)



device = "/dev/ttyACM0" # default device.
log_file = "state_logger_{time_date}.log".format(time_date = get_curr_time_date())
log_error_file = "state_logger_error_{time_date}".format(time_date = get_curr_time_date())

if (len(sys.argv) > 1):
    if (sys.argv[1] == "--help"):
        print("Usage:")
        print("\tpython3 state_encoder.py [device file path]\n")
        print("\tDefault device file path is /dev/ttyACM0")
        exit(0)
    else:
        device = sys.argv[1]

try:
    # open serial port where raspberry pi is.
    with serial.Serial(device) as rasp:
        while True:
            line           = rasp.readline()
            line_split     = str(line.decode("utf-8")).split(" ")
            curr_timestamp = str(int(line_split[0])/1_000_000)
            curr_signals   = line_split[1]
            log_encoded_signals(curr_timestamp, curr_signals, log_file, log_error_file)
except KeyboardInterrupt:
    pass
except:
    print("Failed to open {dev_file}".format(dev_file = device))
