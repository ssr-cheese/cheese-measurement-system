%���������Z���T�ɉ����ď������s
function changeStateBySensor(app)
global got_time sensor_id;
if sensor_id == 0
    % 00 : start�̈�
    startEpochTime = datetime(got_time / 1000, 'ConvertFrom', 'epochtime');
    app.triggerStart(startEpochTime);
    %app.triggerStart(got_time);
elseif sensor_id  == 1
    %01 : goal�̈�
    goalEpochTime = datetime(got_time / 1000, 'ConvertFrom', 'epochtime');
    app.triggerGoal(goalEpochTime);
    %app.triggerGoal(got_time);

end
%�Z���T���ȊO�̂��̂�ǂݎ�����Ƃ��́Csensor_id = ""�ƂȂ��Ă���
sensor_id = ""; %��A���Ŕ������Ȃ��悤�ɖ������
end