%serial readまでまとめた関数
function [] = serialCallback2(src,event,app)

%GUIと共有する変数．取得時刻，センサID
global got_time sensor_id success_flag esp32_serial;
start_time = datetime('now');
%
%ここでしか使わない変数
mark = 1145141919;     %目印
time_length = 5;    %時刻の文字数
id_length = 2;      %IDの文字数


strings = [];
count = [];
msg = [];
%1行読み込み.1行データが無かったら10msでtimeout
%if ~isempty(esp32_serial)
    [data, count, msg] = fscanf(esp32_serial,'%d %d %d');
%end

%読み取り失敗なら無文字列をtime,IDにセット
got_time = "";
sensor_id = "";
success_flag = isempty(msg);

%読み取り成功
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

