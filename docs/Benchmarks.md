This document describes Klipper benchmarks.

Micro-controller Benchmarks
===========================

This section describes the mechanism used to generate the Klipper
micro-controller step rate benchmarks.

The primary goal of the benchmarks is to provide a consistent
mechanism for measuring the impact of coding changes within the
software. A secondary goal is to provide high-level metrics for
comparing the performance between chips and between software
platforms.

The step rate benchmark is designed to find the maximum stepping rate
that the hardware and software can reach. This benchmark stepping rate
is not achievable in day-to-day use as Klipper needs to perform other
tasks (eg, mcu/host communication, temperature reading, endstop
checking) in any real-world usage.

In general, the pins for the benchmark tests are chosen to flash LEDs
or other innocuous pins. **Always verify that it is safe to drive the
configured pins prior to running a benchmark.** It is not recommended
to drive an actual stepper during a benchmark.

## Step rate benchmark test ##

The test is performed using the console.py tool (described in
[Debugging.md](Debugging.md)). The micro-controller is configured for
the particular hardware platform (see below) and then the following is
cut-and-paste into the console.py terminal window:
```
SET start_clock {clock+freq}
SET ticks 1000

reset_step_clock oid=0 clock={start_clock}
set_next_step_dir oid=0 dir=0
queue_step oid=0 interval={ticks} count=60000 add=0
set_next_step_dir oid=0 dir=1
queue_step oid=0 interval=3000 count=1 add=0

reset_step_clock oid=1 clock={start_clock}
set_next_step_dir oid=1 dir=0
queue_step oid=1 interval={ticks} count=60000 add=0
set_next_step_dir oid=1 dir=1
queue_step oid=1 interval=3000 count=1 add=0

reset_step_clock oid=2 clock={start_clock}
set_next_step_dir oid=2 dir=0
queue_step oid=2 interval={ticks} count=60000 add=0
set_next_step_dir oid=2 dir=1
queue_step oid=2 interval=3000 count=1 add=0
```

The above tests three steppers simultaneously stepping. If running the
above results in a "Rescheduled timer in the past" or "Stepper too far
in past" error then it indicates the `ticks` parameter is too low (it
results in a stepping rate that is too fast). The goal is to find the
lowest setting of the ticks parameter that reliably results in a
successful completion of the test. It should be possible to bisect the
ticks parameter until a stable value is found.

On a failure, one can copy-and-paste the following to clear the error
in preparation for the next test:
```
clear_shutdown
```

To obtain the single stepper and dual stepper benchmarks, the same
configuration sequence is used, but only the first block (for the
single stepper case) or first two blocks (for the dual stepper case)
of the above test is cut-and-paste into the console.py window.

To produce the benchmarks found in the Features.md document, the total
number of steps per second is calculated by multiplying the number of
active steppers with the nominal mcu frequency and dividing by the
final ticks parameter. The results are rounded to the nearest K. For
example, with three active steppers:
```
ECHO Test result is: {"%.0fK" % (3. * freq / ticks / 1000.)}
```

### AVR step rate benchmark ###

The following configuration sequence is used on AVR chips:
```
PINS arduino
allocate_oids count=3
config_stepper oid=0 step_pin=ar29 dir_pin=ar28 min_stop_interval=0 invert_step=0
config_stepper oid=1 step_pin=ar27 dir_pin=ar26 min_stop_interval=0 invert_step=0
config_stepper oid=2 step_pin=ar23 dir_pin=ar22 min_stop_interval=0 invert_step=0
finalize_config crc=0
```

The test was last run on commit `b161a69e` with gcc version `avr-gcc
(GCC) 4.8.1`. Both the 16Mhz and 20Mhz tests were run using simulavr
configured for an atmega644p (previous tests have confirmed simulavr
results match tests on both a 16Mhz at90usb and a 16Mhz atmega2560).
On both 16Mhz and 20Mhz the best single stepper result is `SET ticks
106`, the best dual stepper result is `SET ticks 276`, and the best
three stepper result is `SET ticks 481`.

### Arduino Due step rate benchmark ###

The following configuration sequence is used on the Due:
```
allocate_oids count=3
config_stepper oid=0 step_pin=PB27 dir_pin=PA21 min_stop_interval=0 invert_step=0
config_stepper oid=1 step_pin=PB26 dir_pin=PC30 min_stop_interval=0 invert_step=0
config_stepper oid=2 step_pin=PA21 dir_pin=PC30 min_stop_interval=0 invert_step=0
finalize_config crc=0
```

The test was last run on commit `74c21654` with gcc version
`arm-none-eabi-gcc (Fedora 7.1.0-5.fc27) 7.1.0`. The best single
stepper result is `SET ticks 388`, the best dual stepper result is
`SET ticks 405`, and the best three stepper result is `SET ticks 576`.

### Duet Maestro step rate benchmark ###

The following configuration sequence is used on the Duet Maestro:
```
allocate_oids count=3
config_stepper oid=0 step_pin=PC26 dir_pin=PC18 min_stop_interval=0 invert_step=0
config_stepper oid=1 step_pin=PC26 dir_pin=PA8 min_stop_interval=0 invert_step=0
config_stepper oid=2 step_pin=PC26 dir_pin=PB4 min_stop_interval=0 invert_step=0
finalize_config crc=0
```

The test was last run on commit `74c21654` with gcc version
`arm-none-eabi-gcc (Fedora 7.1.0-5.fc27) 7.1.0`. The best single
stepper result is `SET ticks 553`, the best dual stepper result is
`SET ticks 563`, and the best three stepper result is `SET ticks 623`.

### Duet Wifi step rate benchmark ###

The following configuration sequence is used on the Duet Wifi:
```
allocate_oids count=4
config_stepper oid=0 step_pin=PD6 dir_pin=PD11 min_stop_interval=0 invert_step=0
config_stepper oid=1 step_pin=PD7 dir_pin=PD12 min_stop_interval=0 invert_step=0
config_stepper oid=2 step_pin=PD8 dir_pin=PD13 min_stop_interval=0 invert_step=0
config_stepper oid=3 step_pin=PD5 dir_pin=PA1 min_stop_interval=0 invert_step=0
finalize_config crc=0

```

The test was last run on commit `59a60d68` with gcc version
`arm-none-eabi-gcc 7.3.1 20180622 (release)
[ARM/embedded-7-branch revision 261907]`. The best single stepper
result is `SET ticks 519`, the best dual stepper result is `SET ticks
520`, and the best three stepper result is `SET ticks 525`, and the
best four stepper result is `SET ticks 703`.

### Beaglebone PRU step rate benchmark ###

The following configuration sequence is used on the PRU:
```
PINS beaglebone
allocate_oids count=3
config_stepper oid=0 step_pin=P8_13 dir_pin=P8_12 min_stop_interval=0 invert_step=0
config_stepper oid=1 step_pin=P8_15 dir_pin=P8_14 min_stop_interval=0 invert_step=0
config_stepper oid=2 step_pin=P8_19 dir_pin=P8_18 min_stop_interval=0 invert_step=0
finalize_config crc=0
```

The test was last run on commit `b161a69e` with gcc version `pru-gcc
(GCC) 8.0.0 20170530 (experimental)`. The best single stepper result
is `SET ticks 861`, the best dual stepper result is `SET ticks 853`,
and the best three stepper result is `SET ticks 883`.

### STM32F042 step rate benchmark ###

The following configuration sequence is used on the STM32F042:
```
allocate_oids count=3
config_stepper oid=0 step_pin=PA0 dir_pin=PA1 min_stop_interval=0 invert_step=0
config_stepper oid=1 step_pin=PA2 dir_pin=PA3 min_stop_interval=0 invert_step=0
config_stepper oid=2 step_pin=PA6 dir_pin=PA7 min_stop_interval=0 invert_step=0
finalize_config crc=0
```

The test was last run on commit `c105adc8` with gcc version
`arm-none-eabi-gcc (GNU Tools 7-2018-q3-update) 7.3.1`. The best single
stepper result is `SET ticks 308`, the best dual stepper result is
`SET ticks 638`, and the best three stepper result is `SET ticks 1021`.

### STM32F103 step rate benchmark ###

The following configuration sequence is used on the STM32F103:
```
allocate_oids count=3
config_stepper oid=0 step_pin=PC13 dir_pin=PB5 min_stop_interval=0 invert_step=0
config_stepper oid=1 step_pin=PB3 dir_pin=PB6 min_stop_interval=0 invert_step=0
config_stepper oid=2 step_pin=PA4 dir_pin=PB7 min_stop_interval=0 invert_step=0
finalize_config crc=0
```

The test was last run on commit `9f3517fd` with gcc version
`arm-none-eabi-gcc (Fedora 7.1.0-5.fc27) 7.1.0`. The best single
stepper result is `SET ticks 345`, the best dual stepper result is
`SET ticks 365`, and the best three stepper result is `SET ticks 606`.

### STM32F4 step rate benchmark ###

The following configuration sequence is used on the STM32F4:
```
allocate_oids count=4
config_stepper oid=0 step_pin=PA5 dir_pin=PB5 min_stop_interval=0 invert_step=0
config_stepper oid=1 step_pin=PB2 dir_pin=PB6 min_stop_interval=0 invert_step=0
config_stepper oid=2 step_pin=PB3 dir_pin=PB7 min_stop_interval=0 invert_step=0
config_stepper oid=3 step_pin=PB3 dir_pin=PB8 min_stop_interval=0 invert_step=0
finalize_config crc=0
```

The test was last run on commit `c380d463` with gcc version
`arm-none-eabi-gcc (Fedora 7.4.0-1.fc30) 7.4.0`. For the STM32F446 the
best single stepper result is `SET ticks 757`, the best dual stepper
result is `SET ticks 761`, the best three stepper result is `SET ticks
757`, and the best four stepper result is `SET ticks 767`. The
STM32F407 results were obtained by running an STM32F407 binary on an
STM32F446 (and thus using a 168Mhz clock) - the best single stepper
result is `SET ticks 709`, the best dual stepper result is `SET ticks
714`, the best three stepper result is `SET ticks 709`, and the best
four stepper result is `SET ticks 729`.

### LPC176x step rate benchmark ###

The following configuration sequence is used on the LPC176x:
```
allocate_oids count=3
config_stepper oid=0 step_pin=P1.20 dir_pin=P1.18 min_stop_interval=0 invert_step=0
config_stepper oid=1 step_pin=P1.21 dir_pin=P1.18 min_stop_interval=0 invert_step=0
config_stepper oid=2 step_pin=P1.23 dir_pin=P1.18 min_stop_interval=0 invert_step=0
finalize_config crc=0
```

The test was last run on commit `9f3517fd` with gcc version
`arm-none-eabi-gcc (Fedora 7.1.0-5.fc27) 7.1.0`. For the 100Mhz
LPC1768, the best single stepper result is `SET ticks 448`, the best
dual stepper result is `SET ticks 450`, and the best three stepper
result is `SET ticks 523`. The 120Mhz LPC1769 results were obtained by
overclocking an LPC1768 to 120Mhz - the best single stepper result is
`SET ticks 525`, the best dual stepper result is `SET ticks 526`, and
the best three stepper result is `SET ticks 545`.

### SAMD21 step rate benchmark ###

The following configuration sequence is used on the SAMD21:
```
allocate_oids count=3
config_stepper oid=0 step_pin=PA27 dir_pin=PA20 min_stop_interval=0 invert_step=0
config_stepper oid=1 step_pin=PB3 dir_pin=PA21 min_stop_interval=0 invert_step=0
config_stepper oid=2 step_pin=PA17 dir_pin=PA21 min_stop_interval=0 invert_step=0
finalize_config crc=0
```

The test was last run on commit `9f3517fd` with gcc version
`arm-none-eabi-gcc (Fedora 7.1.0-5.fc27) 7.1.0`. The best single
stepper result is `SET ticks 277`, the best dual stepper result is
`SET ticks 410`, and the best three stepper result is `SET ticks 664`.

### SAMD51 step rate benchmark ###

The following configuration sequence is used on the SAMD51:
```
allocate_oids count=4
config_stepper oid=0 step_pin=PA22 dir_pin=PA20 min_stop_interval=0 invert_step=0
config_stepper oid=1 step_pin=PA22 dir_pin=PA21 min_stop_interval=0 invert_step=0
config_stepper oid=2 step_pin=PA22 dir_pin=PA19 min_stop_interval=0 invert_step=0
config_stepper oid=3 step_pin=PA22 dir_pin=PA18 min_stop_interval=0 invert_step=0
finalize_config crc=0
```

The test was last run on commit `9f3517fd` with gcc version
`arm-none-eabi-gcc (Fedora 7.1.0-5.fc27) 7.1.0` on a SAMD51G19A
micro-controller. The best single stepper result is `SET ticks 516`,
the best dual stepper result is `SET ticks 520`, the best three
stepper result is `SET ticks 519`, and the best four stepper result is
`SET ticks 655`.

## Command dispatch benchmark ##

The command dispatch benchmark tests how many "dummy" commands the
micro-controller can process. It is primarily a test of the hardware
communication mechanism. The test is run using the console.py tool
(described in [Debugging.md](Debugging.md)). The following is
cut-and-paste into the console.py terminal window:
```
DELAY {clock + 2*freq} get_uptime
FLOOD 100000 0.0 end_group
get_uptime
```

When the test completes, determine the difference between the clocks
reported in the two "uptime" response messages. The total number of
commands per second is then `100000 * mcu_frequency / clock_diff`.

Note that this test may saturate the USB/CPU capacity of a Raspberry
Pi. The benchmarks below are with console.py running on a desktop
class machine.

| MCU                 | Rate | Build    | Build compiler      |
| ------------------- | ---- | -------- | ------------------- |
| pru (shared memory) |   5K | b161a69e | pru-gcc (GCC) 8.0.0 20170530 (experimental) |
| stm32f042 (CAN)     |  18K | c105adc8 | arm-none-eabi-gcc (GNU Tools 7-2018-q3-update) 7.3.1 |
| atmega2560 (serial) |  23K | b161a69e | avr-gcc (GCC) 4.8.1 |
| sam3x8e (serial)    |  23K | b161a69e | arm-none-eabi-gcc (Fedora 7.1.0-5.fc27) 7.1.0 |
| at90usb1286 (USB)   |  75K | b161a69e | avr-gcc (GCC) 4.8.1 |
| samd21 (USB)        | 238K | b161a69e | arm-none-eabi-gcc (Fedora 7.1.0-5.fc27) 7.1.0 |
| stm32f103 (USB)     | 335K | b161a69e | arm-none-eabi-gcc (Fedora 7.1.0-5.fc27) 7.1.0 |
| sam3x8e (USB)       | 450K | a5aede52 | arm-none-eabi-gcc (Fedora 7.1.0-5.fc27) 7.1.0 |
| lpc1768 (USB)       | 546K | b161a69e | arm-none-eabi-gcc (Fedora 7.1.0-5.fc27) 7.1.0 |
| sam4s8c (USB)       | 619K | a5aede52 | arm-none-eabi-gcc (Fedora 7.1.0-5.fc27) 7.1.0 |
| lpc1769 (USB)       | 619K | b161a69e | arm-none-eabi-gcc (Fedora 7.1.0-5.fc27) 7.1.0 |
| samd51 (USB)        | 620K | 8cd83b4c | arm-none-eabi-gcc (Fedora 7.1.0-5.fc27) 7.1.0 |
| stm32f446 (USB)     | 621K | aac51bdb | arm-none-eabi-gcc (Fedora 7.4.0-1.fc30) 7.4.0 |

Host Benchmarks
===============

It is possible to run timing tests on the host software using the
"batch mode" processing mechanism (described in
[Debugging.md](Debugging.md)). This is typically done by choosing a
large and complex G-Code file and timing how long it takes for the
host software to process it. For example:
```
time ~/klippy-env/bin/python ./klippy/klippy.py config/example.cfg -i something_complex.gcode -o /dev/null -d out/klipper.dict
```
