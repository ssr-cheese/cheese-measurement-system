%���������Z���T�ɉ����ď������s
function changeStateBySensor(app)
global got_time sensor_id;
if sensor_id == "00"
    % 00 : start�̈�
    
    app.triggerStart(got_time);

elseif sensor_id  == "01"
    %01 : goal�̈�
    app.triggerGoal(got_time);

end
%�Z���T���ȊO�̂��̂�ǂݎ�����Ƃ��́Csensor_id = ""�ƂȂ��Ă���
sensor_id = ""; %��A���Ŕ������Ȃ��悤�ɖ������
end