%excute start or goal process 
function changeStateBySensor(app)
global got_time sensor_id;
if sensor_id == 0
    % 0 : start
    startEpochTime = datetime(got_time / 1000, 'ConvertFrom', 'epochtime');
    app.triggerStart(startEpochTime);
    %app.triggerStart(got_time);
elseif sensor_id  == 1
    % 1 : goal
    goalEpochTime = datetime(got_time / 1000, 'ConvertFrom', 'epochtime');
    app.triggerGoal(goalEpochTime);
    %app.triggerGoal(got_time);

end

sensor_id = ""; % every times initilize sensor_id
end