%serial read�܂ł܂Ƃ߂��֐�
function [] = serialCallback2(src,event,app)

%GUI�Ƌ��L����ϐ��D�擾�����C�Z���TID
global got_time sensor_id success_flag esp32_serial;
start_time = datetime('now');
%
%�����ł����g��Ȃ��ϐ�
mark = 1145141919;     %�ڈ�
time_length = 5;    %�����̕�����
id_length = 2;      %ID�̕�����


strings = [];
count = [];
msg = [];
%1�s�ǂݍ���.1�s�f�[�^������������10ms��timeout
%if ~isempty(esp32_serial)
    [data, count, msg] = fscanf(esp32_serial,'%d %d %d');
%end

%�ǂݎ�莸�s�Ȃ疳�������time,ID�ɃZ�b�g
got_time = "";
sensor_id = "";
success_flag = isempty(msg);

%�ǂݎ�萬��
if success_flag
  
    if data(1) == mark
        got_sensor_id = data(2)
        sensor_id = got_sensor_id;
        %got_time = datetime('now');
        got_time = data(3);
        changeStateBySensor(app);
    end
end
end_time = datetime('now');
passed_time = seconds(end_time - start_time)

