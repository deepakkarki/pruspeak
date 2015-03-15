#Examples and documentations

##Pin mappings
DIO[0] to DIO[7] => pru0_r30_0 to pru0_r30_7
pinmux.pdf contains the pru0_r30_x (pru format) GPIOy_z (GPIO format) mappings.
beaglebone_headers.png has the pin number in GPIO format to header pin mapping.

eg. searching for 'pru0_pru_r30_0' [ i.e. DIO[0] ] in pinmux.pdf gives us a corresponding 'gpio3_14'
looking up 'gpio3_14' in beaglebone_headers.png gives us header pin 'P9.31'. 
Hence DIO[0] corresponds to 'P9.31'

Added support for access GPIOs other than __R30. Has to expanded for other GPIO modules (Currently GPIO0)

DIO[8] -> GPIO0_2 -> P9.22
DIO[9] -> GPIO0_3 -> P9.21
DIO[10] -> GPIO0_4 -> P9.18
DIO[11] -> GPIO0_5 -> P9.17
DIO[12] -> GPIO0_7 -> P9.42
DIO[13] -> GPIO0_8 -> P8.35
DIO[14] -> GPIO0_9 -> P8.33
DIO[15] -> GPIO0_10 -> P8.31
DIO[16] -> GPIO0_11 -> P8.32
