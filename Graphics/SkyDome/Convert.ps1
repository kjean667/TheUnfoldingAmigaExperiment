$copperConverter = "..\..\Tools\CopperColorExtractor\bin\Debug\net7.0\CopperColorExtractor.exe"

Get-ChildItem . -Filter *.png |
Foreach-Object {
  $inputName = $_.FullName
  $outputName = $_.BaseName  + ".bin"
  Write-Host $_.BaseName -ForegroundColor Green
  & $copperConverter --input $inputName --output $outputName
}
