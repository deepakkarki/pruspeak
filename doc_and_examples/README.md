#Examples and documentations

##Pin mappings
DIO[0] to DIO[8] => pru0_r30_0 to pru0_r30_8
pinmux.pdf contains the pru0_r30_x (pru format) GPIOy_z (GPIO format) mappings.
beaglebone_headers.png has the pin number in GPIO format to header pin mapping.

eg. searching for 'pru0_pru_r30_0' [ i.e. DIO[0] ] in pinmux.pdf gives us a corresponding 'gpio3_14'
looking up 'gpio3_14' in beaglebone_headers.png gives us header pin 'P9.31'. 
Hence DIO[0] corresponds to 'P9.31'

