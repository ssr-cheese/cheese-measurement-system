%serial read�܂ł܂Ƃ߂��֐�
function [] = serialCallback(src,event,app)

%GUI�Ƌ��L����ϐ��D�擾�����C�Z���TID
global got_time sensor_id success_flag esp32_serial;
start_time = datetime('now');
%
%�����ł����g��Ȃ��ϐ�
mark = 'matlab';     %�ڈ�
time_length = 5;    %�����̕�����
id_length = 2;      %ID�̕�����


strings = [];
count = [];
msg = [];
%1�s�ǂݍ���.1�s�f�[�^������������10ms��timeout
%if ~isempty(esp32_serial)
    [mark, read_time, sensor_id] = fscanf(esp32_serial,'%s %d %d');
%end

%�ǂݎ�莸�s�Ȃ疳�������time,ID�ɃZ�b�g
got_time = "";
sensor_id = "";
success_flag = isempty(msg);

%�ǂݎ�萬��
if success_flag
    mark_last_index = strfind(strings,mark) + length(mark) - 1;
    
    %mark�̒���Ƀf�[�^������
    if (mark_last_index + time_length + id_length + 2) <= length(strings)
        got_time = datetime('now') %str(mark_index + 1 : mark_index + time_length);
        sensor_id = strings( mark_last_index + time_length + 3 : mark_last_index + time_length + id_length + 2)
        
        changeStateBySensor(app);
    end
end
end_time = datetime('now');
passed_time = seconds(end_time - start_time)

