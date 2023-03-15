import serial

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

def print_encoded_signal(timestamp, signal_id, signal_val):
    print(get_log_format(timestamp, signal_id, signal_val))

def write_log_to_file(timestamp, signal_id, signal_val):
    log_file = open("state_logger_logs.log", "a")
    log_file.write(get_log_format(timestamp, signal_id, signal_val))
    log_file.write("\n")
    log_file.close()

def log_encoded_signals(timestamp, signals):
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
    print_encoded_signal(timestamp, "checkpoint error after boot", checkpoint_err_bt)

    # save log to a file.
    write_log_to_file(timestamp, "Checker", encode_checker_signal(checker))
    write_log_to_file(timestamp, "Core standbywfe", core_standbywfe)
    write_log_to_file(timestamp, "Core parity error", core_parity_error)
    write_log_to_file(timestamp, "Watchdog timer expiration", watchdog_timer_ex)
    write_log_to_file(timestamp, "Forced standby", forced_standby)
    write_log_to_file(timestamp, "checkpoint error after boot", checkpoint_err_bt)



# open serial port where raspberry pi is.
with serial.Serial('/dev/ttyACM0') as rasp:
    while True:
        line           = rasp.readline()
        line_split     = str(line.decode("utf-8")).split(" ")
        curr_timestamp = str(int(line_split[0])/1_000_000)
        curr_signals   = line_split[1]
        log_encoded_signals(curr_timestamp, curr_signals)