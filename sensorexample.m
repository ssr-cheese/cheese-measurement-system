clear

app = mousetimer(1, 'だんごろマウスNull', 'dango_bot', '東工大ロ技研Cheese');

% Push "Begin" button on the app, and hit any key on the command window
pause;

% Receive epoch time of the start sensor and trigger the start command
startEpochTime = datetime(100.291, 'ConvertFrom', 'epochtime');
app.triggerStart(startEpochTime);

% Hit any key to proceed
pause;

% Receive epoch time of the goal sensor and trigger the goal command
goalEpochTime = datetime(124.643, 'ConvertFrom', 'epochtime');
app.triggerGoal(goalEpochTime);

% app.saveData