# Install python3 HID package https://pypi.org/project/hid/

import ctypes
try:
    import hid
except ModuleNotFoundError:
    print("Missing import, please try 'pip install hid' or consult your OS's python package manager.")
    exit(1)

# Example must be compiled with CFG_AUDIO_DEBUG=1
VID = 0xcafe
PID = 0x4014

CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX = 2


class audio_debug_info_t(ctypes.Structure):
    _fields_ = [("sample_rate", ctypes.c_uint32),
                ("alt_settings", ctypes.c_uint8),
                ("mute", (CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX + 1) * ctypes.c_int8),
                ("volume", (CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX + 1) * ctypes.c_int16),
                ("fifo_size", ctypes.c_uint16),
                ("fifo_count", ctypes.c_uint16),
                ("fifo_count_avg", ctypes.c_uint16)
                ]


dev = hid.Device(VID, PID)

if dev:
    # Create figure for plotting
    # fig = plt.figure()
    # ax = fig.add_subplot(1, 1, 1)
    fifo_avg = []
    fifo_cnt = []

    # This function is called periodically from FuncAnimation
    while True:
        info = None
        for i in range(30):
            try:
                str_in = dev.read(64, 50)
                info = audio_debug_info_t.from_buffer_copy(str_in)

                fifo_avg.append(info.fifo_count_avg)
                fifo_cnt.append(info.fifo_count)
            except Exception:
                print("Error")
                exit(1)
        # Limit to 1000 items
        fifo_avg = fifo_avg[-1000:]
        fifo_cnt = fifo_cnt[-1000:]

        if info is not None:
            print(f'FIFO size:{info.fifo_size} | FIFO count:{info.fifo_count}')

    # ani = animation.FuncAnimation(fig, animate, interval=10)
    # plt.show()
    print('done')
