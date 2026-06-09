[xml]$xml = Get-Content "test_results/gtest_report.xml"
$html = @"
<html>
<head>
    <style>
        body { font-family: Arial; }
        table { border-collapse: collapse; width: 100%; }
        th, td { border: 1px solid #ccc; padding: 8px; text-align: left; }
        th { background-color: #f2f2f2; }
        .PASSED { color: green; font-weight: bold; }
        .FAILED { color: red; font-weight: bold; }
    </style>
</head>
<body>
    <h2>Google Test 결과 보고서</h2>
    <table>
        <tr><th>Suite</th><th>Test Case</th><th>Result</th><th>Time (s)</th></tr>
"@

foreach ($suite in $xml.testsuites.testsuite) {
    foreach ($case in $suite.testcase) {
        $status = if ($case.failure) { "FAILED" } else { "PASSED" }
        $html += "<tr><td>$($suite.name)</td><td>$($case.name)</td><td class='$status'>$status</td><td>$($case.time)</td></tr>"
    }
}
$html += "</table></body></html>"
Set-Content -Path "outputs/report.html" -Value $html