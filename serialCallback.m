%serial read�܂ł܂Ƃ߂��֐�
function [] = serialCallback(src,event,app)

%GUI�Ƌ��L����ϐ��D�擾�����C�Z���TID
global got_time sensor_id success_flag esp32_serial;

%
%�����ł����g��Ȃ��ϐ�
mark = '@';     %�ڈ�
time_length = 5;    %�����̕�����
id_length = 2;      %ID�̕�����


strings = [];
count = [];
msg = [];
%1�s�ǂݍ���.1�s�f�[�^������������10ms��timeout
%if ~isempty(esp32_serial)
    [strings,count,msg] = fgetl(esp32_serial);
%end

%�ǂݎ�莸�s�Ȃ疳�������time,ID�ɃZ�b�g
got_time = "";
sensor_id = "";
success_flag = isempty(msg);

%�ǂݎ�萬��
if success_flag
    mark_index = strfind(strings,mark);
    
    %mark�̒���Ƀf�[�^������
    if (mark_index + time_length + id_length + 1) <= length(strings)
        got_time = datetime('now') %str(mark_index + 1 : mark_index + time_length);
        sensor_id = strings( mark_index + time_length + 2 : mark_index + time_length + id_length + 1)
        
        changeStateBySensor(app);
    end
end
