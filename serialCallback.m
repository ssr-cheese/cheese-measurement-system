%serial readまでまとめた関数
function [] = serialCallback(src,event,app)

%GUIと共有する変数．取得時刻，センサID
global got_time sensor_id success_flag esp32_serial;
start_time = datetime('now');
%
%ここでしか使わない変数
mark = 'matlab';     %目印
time_length = 5;    %時刻の文字数
id_length = 2;      %IDの文字数


strings = [];
count = [];
msg = [];
%1行読み込み.1行データが無かったら10msでtimeout
%if ~isempty(esp32_serial)
    [mark, read_time, sensor_id] = fscanf(esp32_serial,'%s %d %d');
%end

%読み取り失敗なら無文字列をtime,IDにセット
got_time = "";
sensor_id = "";
success_flag = isempty(msg);

%読み取り成功
if success_flag
    mark_last_index = strfind(strings,mark) + length(mark) - 1;
    
    %markの直後にデータが存在
    if (mark_last_index + time_length + id_length + 2) <= length(strings)
        got_time = datetime('now') %str(mark_index + 1 : mark_index + time_length);
        sensor_id = strings( mark_last_index + time_length + 3 : mark_last_index + time_length + id_length + 2)
        
        changeStateBySensor(app);
    end
end
end_time = datetime('now');
passed_time = seconds(end_time - start_time)

