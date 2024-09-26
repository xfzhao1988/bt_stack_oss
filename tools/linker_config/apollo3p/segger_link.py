# *****************************************************************************
#
#    segger_link.py
#
#    @brief Generate the Segger link control file.
#
# *****************************************************************************

# *****************************************************************************
#
#    Copyright (c) 2024, Ambiq Micro, Inc.
#    All rights reserved.
#
#    Redistribution and use in source and binary forms, with or without
#    modification, are permitted provided that the following conditions are met:
#
#    1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
#    2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
#    3. Neither the name of the copyright holder nor the names of its
#    contributors may be used to endorse or promote products derived from this
#    software without specific prior written permission.
#
#    Third party software included in this distribution is subject to the
#    additional license terms as defined in the /docs/licenses directory.
#
#    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
#    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#    POSSIBILITY OF SUCH DAMAGE.
#
#  This is part of revision release_sdk_3_2_0-dd5f40c14b of the AmbiqSuite Development Package.
#
# *****************************************************************************

from string import Template
import textwrap

def fix_startup_file(config, filename):

    # Get the stack size from the config file. Note that the startup file
    # measures in words, not bytes.
    stack_size = config['STACK']['size']

    # Read in every line of the given file
    input_lines = None
    with open(filename) as f:
        input_lines = [line for line in f]

    # Perform a search-and-replace with the new stack size.
    def fix_line(line):
        return re.sub(r'(Stack   EQU     )0x[0-9a-fA-F]*',
                      r'\g<1>' + '0x{:08X}'.format(stack_size),
                      line)

    output_lines = map(fix_line, input_lines)
    output_text = ''.join(output_lines)

    # Write the new file back to the input
    # Specify the newline parameter to output with Unix line endings.
    with open(filename, 'w', newline='\n') as f:
        f.write(output_text)


def generate_link_script(config):
    ro_base = config['ROMEM']['start']
    ro_size = config['ROMEM']['size']
    rw_base = config['RWMEM']['start']
    rw_size = config['RWMEM']['size']
    tcm_base = config['TCM']['start']
    tcm_size = config['TCM']['size']
    stack_base = config['STACK']['start']
    stack_size = config['STACK']['size']

    D = dict()
    D['ro_base'] = format_hex(ro_base)
    D['ro_size'] = format_hex(ro_size)
    D['rw_base'] = format_hex(rw_base)
    D['rw_size'] = format_hex(rw_size)
    D['tcm_base'] = format_hex(tcm_base)
    D['tcm_size'] = format_hex(tcm_size)

    D['stack_base'] = format_hex(stack_base)

    D['stack_size'] = config['STACK']['size']

    #D['section_defs'] = format_sections(section_definition, config)
    #D['section_blocks'] = format_sections(section_block, config)
    #D['section_placements'] = format_sections(section_placement, config)

    return link_script_template.substitute(**D)


def format_sections(template, config):

    def fill_section_template(section):
        mapping = {
            'section_name': section,
            'block_name': section + '_BLOCK',
            'mem_name': section + '_MEM',
            'align': 4,
            'start': format_hex(config[section]['start']),
            'size': format_hex(config[section]['size']),
            'end': format_hex(config[section]['start'] +
                              config[section]['size']),

            'permissions': convert_permissions(config[section]['perm']),
        }

        return template.substitute(**mapping)

    def is_extra_section(section):
        return section not in required_sections

    sections = filter(is_extra_section, config)

    return ''.join(map(fill_section_template, sections))


def convert_permissions(perm_string):
    if 'r' in perm_string and 'w' in perm_string and 'x' in perm_string:
        return 'readwrite'
    else:
        return 'readonly'


def format_hex(n):
    return '0x{:08X}'.format(n)


link_script_template = Template('''\
//******************************************************************************
//
// Linker file for Segger Embedded Studio linker configuration.
//
//******************************************************************************

// Define Memory Regions
define memory with size = 4G;
define region ROMEM      = [from ${ro_base} size ${ro_size}];
define region RWMEM      = [from ${rw_base} size ${rw_size}];
define region TCM        = [from ${tcm_base} size ${tcm_size}];
define region STACKMEM   = [from ${stack_base} size ${stack_size}];

// Thread-local-storage load image
define block tbss                           { section .tbss,      section .tbss.*  };
define block tdata                          { section .tdata,     section .tdata.* };
define block tls with fixed order           { block tbss, block tdata };
define block tdata_load                     { copy of block tdata };

// Define Blocks
define block heap  with size = 0, alignment = 8, readwrite access { };
define block stack with size = ${stack_size}, alignment = 8, readwrite access { };

define block FLASHBASE                      { section .patch };

// Configure Initialization
do not initialize                       { section .non_init, section .non_init.*};
initialize by copy with packing=auto    { section .data, section .data.* };
initialize by symbol __SEGGER_init_heap { block heap  };

// Place Inputs
// Place Inputs
place at start of ROMEM              { section .vectors, block FLASHBASE };
place in ROMEM                       { readexec, readonly };
place in ROMEM                       { block tdata_load };
place in RWMEM with auto order        { block heap, block tls, readwrite, zeroinit };
place in STACKMEM with fixed order       { block stack };
place in TCM { section .tcm };
''')

section_definition = Template('''\
define region ${mem_name} = [from ${start} size ${size}];
''')

section_block = Template('''\
define block ${block_name} with alignment = ${align}, size = ${size}
{
    ${permissions} section ${section_name}
};
''')

section_placement = Template('''\
place in ${mem_name} { block ${block_name} };
''')