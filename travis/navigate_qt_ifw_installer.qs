/**
* Navigate through the installer for the QT Installer Framework
*
* Thanks to https://github.com/sgsaenger/vipster for the example!
**/

function Controller() {
    installer.autoRejectMessageBoxes();
    installer.installationFinished.connect(function() {
        gui.clickButton(buttons.NextButton);
    })
}

Controller.prototype.WelcomePageCallback = function(){
    gui.clickButton(buttons.NextButton, 3000);
}

Controller.prototype.IntroductionPageCallback = function(){
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.TargetDirectoryPageCallback = function(){
    gui.currentPageWidget().TargetDirectoryLineEdit.setText(installer.value("HomeDir") + "/Qt/QtIFW-3.0.6");
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.LicenseAgreementPageCallback = function(){
    gui.currentPageWidget().AcceptLicenseRadioButton.setChecked(true);
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.ReadyForInstallationPageCallback = function(){
    gui.clickButton(buttons.NextButton);
}

Controller.prototype.FinishedPageCallback = function(){
    gui.clickButton(buttons.FinishButton);
}
