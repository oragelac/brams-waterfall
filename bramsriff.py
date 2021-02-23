#! /usr/bin/env python3
import numpy as np
import struct


class BRAMSHeader:
    """
    This is the first BRAMS subchunk (BRA1) in the audio WAV files.
    """

    fmt_main = "<H 2d 2Q 5d 2H 5d 6s 6s 6s 234s"
    fmt_reserve = " 256s"
    fmt = fmt_main + fmt_reserve

    def __init__(self, buffer):
        (
            self.version,
            self.sample_rate,
            self.lo_freq,
            self.start_us,
            self.pps_count,
            self.beacon_lat,
            self.beacon_long,
            self.beacon_alt,
            self.beacon_freq,
            self.beacon_power,
            self.beacon_polar,
            self.ant_id,
            self.ant_lat,
            self.ant_long,
            self.ant_alt,
            self.ant_az,
            self.ant_el,
            self.beacon_code,
            self.observer_code,
            self.station_code,
            self.description) = struct.unpack(
                self.fmt_main,
                buffer[0:struct.calcsize(self.fmt_main)])

    def show(self):
        print(
            "ver: {} sample rate: {:7.3f} [Sa/s] "
            "LO freq: {:7.4f} [Hz]".format(
                self.version, self.sample_rate,
                self.lo_freq / 1E6))
        print(
            "\nBeacon {} \n"
            "lat: {:7.5f} [deg] long: {:7.5f} [deg]"
            " alt: {:4.0f} [m] freq: {:7.4f}[MHz]".format(
                self.beacon_code.decode('ascii'),
                self.beacon_lat, self.beacon_long,
                self.beacon_alt, self.beacon_freq / 1E6))
        print(
            "\nStation {}\n"
            "lat: {:7.5f} [deg] long: {:7.5f} [deg]"
            " alt: {:4.0f} [m] az: {:4.0f} [deg] "
            "el: {:4.0f} [deg]".format(
                self.station_code.decode('ascii'),
                self.ant_lat, self.ant_long, self.ant_alt,
                self.ant_az, self.ant_el))
        print(
            "\nObserver {}\n"
            "description: {}\n".format(
                self.observer_code.decode('ascii'),
                self.description.decode('ascii')))


class BRAMSPPS:
    """
    This is the second BRAMS subchunk (BRA2) in the audio WAV files.
    """

    fmt = "Q Q"

    def __init__(self, buffer):
        pps = struct.unpack(
            self.fmt * int(len(buffer) / struct.calcsize(self.fmt)),
            buffer)
        self.index = np.array(pps[0::2])
        self.time = np.array(pps[1::2])
        self.dt = np.diff(self.time)
        self.di = np.diff(self.index)

    def show_stats(self):
        print(
            "dt [s] mean: {:5.3f} max: {:5.3f} "
            "min: {:5.3f} std: {:5.3f}".format(
                self.dt.mean() / 1E6,
                self.dt.max() / 1E6,
                self.dt.min() / 1E6,
                self.dt.std() / 1E6))
        print("di mean: {:9.5f} max: {} min: {} std: {:5.3f}".format(
            self.di.mean(),
            self.di.max(),
            self.di.min(),
            self.di.std()))


class BRAMSData:
    """
    This is the data subchunk in the audio WAV files.
    """

    fmt = "h"

    def __init__(self, buffer):
        self.npoints = int(len(buffer) / struct.calcsize(self.fmt))
        self.signal = np.array(struct.unpack(
            "<" + self.fmt * self.npoints,
            buffer))

    def show_stats(self):
        signal_max = self.signal.max()
        signal_min = self.signal.min()
        print(
            "signal [ADU] mean: {:7.1f} max: {:6.0f} "
            "min: {:6.0f} std: {:5.3f}".format(
                self.signal.mean(),
                signal_max,
                signal_min,
                self.signal.std()))
        print(
            "# saturated: {}".format(
                ((self.signal == signal_max) |
                 (self.signal == signal_min)).sum()))


class WAVFile:

    def __init__(self, filename):
        self.filename = filename

    def read(self, show=False):

        brams_header = None
        brams_pps = None
        brams_data = None
        with open(self.filename, 'rb') as fh:
            riff, size, fformat = struct.unpack('<4sI4s', fh.read(12))
            if show:
                print("chunk: {}, Chunk Size: {}, format: {}".format(
                    riff.decode('ascii'), size, fformat.decode('ascii')))
            # Read header
            chunk_header = fh.read(8)
            subchunkid, subchunksize = struct.unpack('<4sI', chunk_header)
            if show:
                print("\nfound subchunk: {} size:{}".format(
                    subchunkid.decode('ascii'), subchunksize))

            if (subchunkid == b'fmt '):
                aformat, channels, samplerate, byterate, blockalign, bps =\
                    struct.unpack('HHIIHH', fh.read(16))
                bitrate = (samplerate * channels * bps) / 1024
                if show:
                    print(
                        "\tFormat: {}, Channels {}, "
                        "Sample Rate: {}, Kbps: {}".format(
                            aformat, channels, samplerate, bitrate))

            chunkOffset = fh.tell()

            # go through main chunk
            while (chunkOffset < size):
                fh.seek(chunkOffset)
                subchunkid, subchunksize = struct.unpack('<4sI', fh.read(8))
                if show:
                    print("chunk id: {}, size: {}".format(
                        subchunkid, subchunksize))
                if (subchunkid == b'BRA1'):
                    if not (subchunksize == struct.calcsize(BRAMSHeader.fmt)):
                        print("\n\tERROR incorrrect subchunksize !\n")
                    brams_header = BRAMSHeader(fh.read(
                        struct.calcsize(BRAMSHeader.fmt_main)))
                    if show:
                        brams_header.show()
                    fh.seek(struct.calcsize(BRAMSHeader.fmt_reserve), 1)
                elif (subchunkid == b'data'):
                    brams_data = BRAMSData(fh.read(subchunksize))
                    if show:
                        print("Found data")

                elif (subchunkid == b'BRA2'):
                    brams_pps = BRAMSPPS(fh.read(subchunksize))
                else:
                    pass
                chunkOffset += (subchunksize + 8)
            return(brams_header, brams_pps, brams_data)


if __name__ == "__main__":
    
    import argparse
    import matplotlib.pyplot as plt

    parser = argparse.ArgumentParser(
        description='Find beacon and calibrator every second from a wav file')
    parser.add_argument('filename', type=str, help='wav file name')
    parser.add_argument(
        "-s", "--show", help="print chunk and subchunk info",
        action="store_true")
    args = parser.parse_args()

    # print(args)
    wavFile = WAVFile(args.filename)
    b, pps, data = wavFile.read(args.show)

    import socket
    import time

    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind(('', 4321))
    server.listen(1)

    while True:

        client, address = server.accept()

        while True:

            for sample in data.signal:

                client.send(struct.pack("<h", sample))
                time.sleep(0.00005)

        client.close()

    

