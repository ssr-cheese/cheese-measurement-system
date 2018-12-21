%serial callback reading
function [] = serialCallback2(src,event,app)

%the variants shared by serialInitialize.m, serialEnd.m , changeStateBySensor.m
global got_time sensor_id success_flag esp32_serial;
start_time = datetime('now');
%

%mezirushi
mark = 1145141919;     %

strings = [];
count = [];
msg = [];

%One line reading from serial object.
%if ~isempty(esp32_serial)
    [data, count, msg] = fscanf(esp32_serial,'%d %d %d');
%end


%msg is error message. If msg is empty, reading is success.
%If reading is succses, data = (3 x 1) vector
got_time = "";
sensor_id = "";
success_flag = isempty(msg);

%
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

