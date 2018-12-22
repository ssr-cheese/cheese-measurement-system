function duration_time = inverseFormatTime(str)

min_str = str2double( str(1:2) );
sec_str = str2double( str(4:5) );
subsec_str = str2double( str(7:9) );
duration_time = min_str * 60 + sec_str + subsec_str / 1000;
