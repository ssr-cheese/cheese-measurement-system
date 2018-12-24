%cheese time application m_file included all function
clear;

%all serial object is closed.
delete(instrfindall);

api_accessor = ApiAccessor("half"); % Please select "half" or "classic" manually.

% user manualy change
runner_number = 11;


%get player data
try
    [mouse_name, player_name, org_name] = api_accessor.getDetail(runner_number);
catch Err
    warning(Err.message);
    warning("The program will be forcibly terminated.");
    return; %program is finished.
end
app = mousetimer(runner_number,mouse_name, player_name, org_name);

%User must chage select appropriate COM port manually.
serialInitialize('COM7',115200,app);

%Wait for a player to complete measurement. If it's completed, you must press any key.
while isvalid(app)
    pause(1);
end

try
    %If app is closed, the run robot data is upload to spread sheet.
    uploadToWeb(api_accessor, runner_number);
    
catch Err
    warning(Err.message);
    warning("Please retry excuting function 'uploadToWeb(api_accessor,runner_number);' from comannd line.");
end

serialEnd;

