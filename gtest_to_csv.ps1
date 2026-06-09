[xml]$xml = Get-Content "test_results/gtest_report.xml"
$results = @()

foreach ($suite in $xml.testsuites.testsuite) {
    foreach ($case in $suite.testcase) {
        $status = if ($case.failure) { "FAIL" } else { "PASS" }
        $obj = [PSCustomObject]@{
            "Test Suite"  = $suite.name
            "Test Case"   = $case.name
            "Status"      = $status
            "Time (sec)"  = $case.time
            "Error Msg"   = if ($case.failure) { $case.failure.message } else { "" }
        }
        $results += $obj
    }
}
# Export-Csv를 사용할 때 -Encoding UTF8 또는 BigEndianUnicode를 주면 엑셀에서 한글이 깨지지 않습니다.
$results | Export-Csv -Path "outputs/GTest_Report.csv" -NoTypeInformation -Encoding UTF8