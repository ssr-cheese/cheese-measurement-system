%cheese time application m_file included all function
clear;

%all serial object is closed.
delete(instrfindall);

api_accessor = ApiAccessor;

% user manualy change
runner_number = 1;


%get player data
[mouse_name, player_name, org_name] = api_accessor.getDetail(runner_number);

app = mousetimer(runner_number - 1,mouse_name, player_name, org_name);

%User must chage select appropriate COM port manually.
serialInitialize('COM5',115200,app);

%Wait for a player to complete measurement. If it's completed, you must press any key.
while isvalid(app)
    pause;  
end

%If app is closed, the run robot data is upload to spread sheet.
uploadToWeb(api_accessor, runner_number);

serialEnd;

