$xmlFiles = Get-ChildItem -Path "build/reports" -Filter "*.xml" -Recurse
$results = @()

if ($xmlFiles.Count -eq 0) {
    Write-Error "파싱할 XML 파일이 존재하지 않습니다."
    exit 1
}

foreach ($file in $xmlFiles) {
    [xml]$xml = Get-Content $file.FullName

    # 루트가 testsuites 또는 testsuite 인 경우를 모두 지원
    $suiteNodes = @()
    if ($xml.testsuites -and $xml.testsuites.testsuite) {
        $suiteNodes = @($xml.testsuites.testsuite)
    } elseif ($xml.testsuite) {
        $suiteNodes = @($xml.testsuite)
    }

    foreach ($suite in $suiteNodes) {
        foreach ($case in $suite.testcase) {
            $status = if ($case.failure) { "FAIL" } else { "PASS" }
            $obj = [PSCustomObject]@{
                "Source File"    = $file.Name  # 여러 파일 구분용 열 추가
                "Test Suite"     = $suite.name
                "Test Case"      = $case.name
                "Status"         = $status
                "Duration (sec)" = $case.time
                "Error Message"  = if ($case.failure) { $case.failure.message.Trim() } else { "" }
            }
            $results += $obj
        }
    }
}

if (!(Test-Path "outputs")) { New-Item -ItemType Directory -Path "outputs" | Out-Null }
# UTF-8 BOM 인코딩으로 엑셀 한글 깨짐 방지
$results | Export-Csv -Path "outputs/GTest_Excel_Report.csv" -NoTypeInformation -Encoding utf8BOM
Write-Host "통합 Excel용 CSV 레포트 생성 완료!"