PATH=%PATH%;%WSDK81%\bin\x86

rem sign using SHA-1
signtool sign /v /sha1 02903e55b5b7877154ac9f2de48f64b00ca3d755 /ac DigiCert_High_Assurance_Code_Signing_CA.cer /fd sha1 /t http://timestamp.verisign.com/scripts/timestamp.dll "..\bin\entropyCalc.exe" "..\bin\entropyCalc-x64.exe" "..\bin\entropyCalc-arm64.exe" "..\bin\entropyCalc-arm.exe"
rem sign using SHA-256
signtool sign /v /sha1 04141E4EA6D9343CEC994F6C099DC09BDD8937C9 /ac GlobalSign_SHA256_EV_CodeSigning_CA.cer /as /fd sha256 /tr http://timestamp.globalsign.com/?signature=sha2 /td SHA256 "..\bin\entropyCalc.exe" "..\bin\entropyCalc-x64.exe" "..\bin\entropyCalc-arm64.exe" "..\bin\entropyCalc-arm.exe"

pause
