function [time,sensor_id] = parseForCheese(str,mark,time_length,id_length)

mark_index = strfind(str,mark);

%markの直後にデータが存在
if (mark_index + time_length + id_length + 1) <= length(str)
    time = str(mark_index + 1 : mark_index + time_length);
    sensor_id = str( mark_index + time_length + 2 : mark_index + time_length + id_length + 1);
else
    time = "";
    sensor_id = "";
end
%文字列形式が，"mark:time:sensor_id"となっていると仮定
%もし，送られてきたデータがおかしくって，次の行にまたがって存在したら？

