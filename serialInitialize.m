function serialInitialize(com_name,baudrate,app)
global esp32_serial
%serialObject初期化
%COMは環境に合わせて変える必要ありそう
%timerより先にシリアル初期化しないと，timerHandleが起動して初期化前にシリアル読み込みしちゃう
serial_list = seriallist;
if ~isempty( find(serial_list == com_name,1) )
    esp32_serial = serial(com_name,'BaudRate',baudrate);
    esp32_serial.BytesAvailableFcn = @(src,event)serialCallback2(src,event,app);
    fopen(esp32_serial);
else
    serial_result = "serial failed" %serial失敗した時は，とりあえず失敗を表示だけはしとく
    %close;
end
