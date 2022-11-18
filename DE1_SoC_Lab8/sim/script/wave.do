onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -divider -height 30 " TB"
add wave -noupdate  -radix binary /low_pass_filter_tb/clk_tb
add wave -noupdate  -radix binary /low_pass_filter_tb/reset_n_tb
add wave -noupdate -radix hexadecimal /low_pass_filter_tb/data_in_tb
add wave -noupdate -radix binary /low_pass_filter_tb/filter_en_tb
add wave -noupdate -radix binary /low_pass_filter_tb/data_out_tb
add wave -divider -height 30 " UUT"
add wave -noupdate -radix binary /low_pass_filter_tb/UUT/clk_tb
add wave -noupdate -radix binary /low_pass_filter_tb/UUT/reset_n_tb
add wave -noupdate -radix hexadecimal  /low_pass_filter_tb/UUT/data_in_tb
add wave -noupdate -radix hexadecimal /low_pass_filter_tb/UUT/filter_en_tb
add wave -noupdate -radix binary /low_pass_filter_tb/UUT/data_out_tb
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 1} {793569513 ps} 0}
quietly wave cursor active 1
configure wave -namecolwidth 150
configure wave -valuecolwidth 100
configure wave -justifyvalue left
configure wave -signalnamewidth 1
configure wave -snapdistance 10
configure wave -datasetprefix 0
configure wave -rowmargin 4
configure wave -childrowmargin 2
configure wave -gridoffset 0
configure wave -gridperiod 1
configure wave -griddelta 40
configure wave -timeline 0
configure wave -timelineunits ns
update
WaveRestoreZoom {0 ps} {1570490714 ps}