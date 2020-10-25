function Component()
{
}
Component.prototype.isDefault = function()
{
return true;
}
Component.prototype.createOperations = function()
{
try
{
component.createOperations();
}
catch (e)
{
print(e);
}
if (installer.value("os") === "win")
{
component.addOperation("CreateShortcut", "@TargetDir@/bin/LibOrg.exe", "@DesktopDir@/LibOrg.lnk");
}
if (installer.value("os") === "x11")
{
component.addOperation("CreateDesktopEntry", "/usr/share/applications/LibOrg.desktop", "Version=1.0\nType=Application\nTerminal=false\nExec=@TargetDir@/LibOrg.sh\nName=LibOrg\nIcon=@TargetDir@YourApp_icon.png\nName[en_US]=YourApp_name");
component.addElevatedOperation("Copy", "/usr/share/applications/LibOrg.desktop", "@HomeDir@/Desktop/LibOrg.desktop");
}
}