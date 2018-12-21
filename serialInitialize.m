function serialInitialize(com_name,baudrate,app)
global esp32_serial
%serialObject initialize

serial_list = seriallist;
if ~isempty( find(serial_list == com_name,1) )
    esp32_serial = serial(com_name,'BaudRate',baudrate);
    esp32_serial.BytesAvailableFcn = @(src,event)serialCallback2(src,event,app);
    fopen(esp32_serial);
else
    serial_result = "serial failed" %debug infor
    %close;
end
