# build/reports/ 폴더 내의 모든 xml 파일을 가져옴
$xmlFiles = Get-ChildItem -Path "build/reports" -Filter "*.xml" -Recurse

if ($xmlFiles.Count -eq 0) {
    Write-Error "파싱할 XML 파일이 존재하지 않습니다."
    exit 1
}

$html = @"
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Integrated Google Test Report</title>
    <style>
        body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; margin: 20px; background-color: #f5f6f8; }
        h2 { color: #1a365d; border-bottom: 2px solid #2b6cb0; padding-bottom: 8px; }
        .source-file { font-size: 12px; color: #718096; margin-top: 15px; margin-bottom: 5px; font-weight: bold; }
        table { border-collapse: collapse; width: 100%; background: white; box-shadow: 0 1px 3px rgba(0,0,0,0.1); margin-bottom: 25px; }
        th, td { border: 1px solid #e2e8f0; padding: 12px; text-align: left; }
        th { background-color: #edf2f7; color: #2d3748; font-weight: bold; }
        tr:nth-child(even) { background-color: #f7fafc; }
        .PASSED { color: #38a169; font-weight: bold; }
        .FAILED { color: #e53e3e; font-weight: bold; background-color: #fff5f5; }
        pre { background: #2d3748; color: #fff; padding: 10px; border-radius: 4px; font-size: 13px; white-space: pre-wrap; }
    </style>
</head>
<body>
    <h2>📊 통합 Google Test 결과 보고서</h2>
"@

# 발견된 모든 XML 파일을 순회하며 하나의 HTML에 누적
foreach ($file in $xmlFiles) {
    [xml]$xml = Get-Content $file.FullName
    
    # 어떤 XML 파일에서 나온 결과인지 표시
    $html += "<div class='source-file'>📄 원본 파일: $($file.Name)</div>"
    $html += "<table>
        <tr>
            <th width='30%'>Test Suite</th>
            <th width='40%'>Test Case</th>
            <th width='15%'>Result</th>
            <th width='15%'>Duration (sec)</th>
        </tr>"

    foreach ($suite in $xml.testsuites.testsuite) {
        foreach ($case in $suite.testcase) {
            $status = if ($case.failure) { "FAILED" } else { "PASSED" }
            $html += "<tr>
                <td>$($suite.name)</td>
                <td>$($case.name)</td>
                <td><span class='$status'>$status</span></td>
                <td>$($case.time)</td>
            </tr>"
            
            if ($case.failure) {
                $html += "<tr><td colspan='4'><pre>$($case.failure.message)</pre></td></tr>"
            }
        }
    }
    $html += "</table>"
}

$html += "</body></html>"

if (!(Test-Path "build/reports")) { New-Item -ItemType Directory -Path "build/reports" | Out-Null }
Set-Content -Path "build/reports/Test_Report.html" -Value $html -Encoding UTF8
Write-Host "통합 HTML 레포트 생성 완료!"