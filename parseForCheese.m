function [time,sensor_id] = parseForCheese(str,mark,time_length,id_length)

mark_index = strfind(str,mark);

%mark�̒���Ƀf�[�^������
if (mark_index + time_length + id_length + 1) <= length(str)
    time = str(mark_index + 1 : mark_index + time_length);
    sensor_id = str( mark_index + time_length + 2 : mark_index + time_length + id_length + 1);
else
    time = "";
    sensor_id = "";
end
%������`�����C"mark:time:sensor_id"�ƂȂ��Ă���Ɖ���
%�����C�����Ă����f�[�^�������������āC���̍s�ɂ܂������đ��݂�����H

