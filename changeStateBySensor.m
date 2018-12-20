%反応したセンサに応じて処理実行
function changeStateBySensor(app)
global got_time sensor_id;
if sensor_id == "00"
    % 00 : startの意
    
    app.triggerStart(got_time);

elseif sensor_id  == "01"
    %01 : goalの意
    app.triggerGoal(got_time);

end
%センサ情報以外のものを読み取ったときは，sensor_id = ""となっている
sensor_id = ""; %二連続で反応しないように毎回消去
end