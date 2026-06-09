pipeline {
    // 사용 가능한 Jenkins 에이전트에서 실행한다.
    agent any
    // 윈도우 mingw(winlibs) 환경변수 경로 명시. -- PATH(사용자) 에 등록했더니 인식 안됬음. 시스템 PATH에 등록하면 인식 될 걸로 예상됨.
    environment {
        // ⚠️ 본인의 실제 winlibs 내부 bin 폴더 절대 경로로 수정하세요!
        // 백슬래시(\)는 반드시 두 번씩(\\) 적어야 문법 에러가 안 납니다.
        WINLIBS_PATH = 'C:\\BuildTools\\winlibs-x86_64-mcf-seh-gcc-13.1.0-mingw-w64ucrt-11.0.0-r5\\mingw64\\bin' 
        
        // 기존 젠킨스 PATH 맨 앞에 WinLibs 경로를 강제로 끼워 넣습니다.
        PATH = "${WINLIBS_PATH};${env.PATH}"
    }

    stages {
        // CMake 구성 단계: 빌드 시스템 파일을 생성한다.
        stage('Configure') {
            steps {
                script {
                    // OS에 따라 셸 명령(sh/bat)을 분기한다.
                    if (isUnix()) {
                        sh 'cmake -S . -B build -DCMAKE_BUILD_TYPE=Release'
                    } else {
                        bat "cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -G \"MinGW Makefiles\""
                    }
                }
            }
        }


        stage('Initialize') {
            steps {
                // 빌드 시작 전 구형 파일 청소 (에러 방지)
                bat 'if exist build\\reports del /f /q build\\reports\\*.xml'
                bat 'if exist outputs del /f /q outputs\\*.*'
            }
        }


        // 컴파일 단계: 실행 파일/테스트 바이너리를 빌드하고 리포트 폴더를 준비한다.
        stage('Build') {
            steps {
                script {
                    if (isUnix()) {
                        sh 'cmake --build build --config Release --parallel'
                        sh 'mkdir -p build/reports'
                    } else {
                        bat 'cmake --build build --config Release --parallel'
                        bat 'if not exist build\\reports mkdir build\\reports'
                    }
                }
            }
        }

        // 단위 테스트 단계: GoogleTest XML 결과를 Jenkins가 읽을 위치에 저장한다.
        stage('Unit Test') {
            steps {
                script {
                    if (isUnix()) {
                        sh './build/bin/unit_tests --gtest_output=xml:build/reports/unit-tests.xml'
                    } else {
                        bat 'build\\bin\\unit_tests.exe --gtest_output=xml:build\\reports\\unit-tests.xml'
                    }
                }
            }
        }

        // 통합 테스트 단계: 단위 테스트와 동일하게 XML 리포트를 생성한다.
        stage('Integration Test') {
            steps {
                script {
                    if (isUnix()) {
                        sh './build/bin/integration_tests --gtest_output=xml:build/reports/integration-tests.xml'
                    } else {
                        bat 'build\\bin\\integration_tests.exe --gtest_output=xml:build\\reports\\integration-tests.xml'
                    }
                }
            }
        }
        
         // 🚀 [핵심] 파이썬 없이 윈도우 자체 기능으로 제출용 HTML 보고서 생성
        // stage('Generate Report File') {
        //     steps {
        //         echo 'Converting XML to Submission-ready HTML via PowerShell...'
        //         powershell """
        //             # ⚠️ $xslt 앞에 백슬래시를 붙여 그루비 가로채기를 방지합니다.
        //             \$xslt = New-Object System.Xml.Xsl.XslCompiledTransform
        //             \$xslt.Load('${workspace}\\gtest-report.xsl')
                    
        //             # 유닛테스트 보고서 변환
        //             \$xslt.Transform('${workspace}\\build\\reports\\report_unit.xml', '${workspace}\\build\\reports\\unit_report.html')
                    
        //             # 통합테스트 보고서 변환
        //             \$xslt.Transform('${workspace}\\build\\reports\\report_int.xml', '${workspace}\\build\\reports\\integration_report.html')
        //         """
        //     }
        // }

        // XML 결과를 사람이 읽기 쉬운 HTML/CSV로 변환한다.
        stage('Generate Reports') {
            steps {
                // 여러 XML 파일을 하나의 통합 보고서로 생성한다.
                powershell '.\\gtest_to_html.ps1'
                powershell '.\\gtest_to_csv.ps1'
            }
        }
    }

    post {
        always {
            // 테스트 실패 여부와 무관하게 XML 리포트를 수집해 Jenkins 테스트 리포트로 표시한다.
            junit allowEmptyResults: true, testResults: 'build/reports/*.xml'
            // 사후 분석을 위해 XML 리포트를 아티팩트로 항상 보관한다.
            archiveArtifacts artifacts: 'build/reports/*.xml, build/reports/*.html, outputs/*.csv', onlyIfSuccessful: false
            
            // HTML Publisher(젠킨스 플러그인 설치 필요)로 빌드 화면에 보고서 탭을 만든다.
            publishHTML([
                allowMissing: true,              // 파일이 혹시 없어도 빌드를 실패시키지 않음
                alwaysLinkToLastBuild: true,     // 메인 화면에서 항상 최신 빌드 리포트로 연결
                keepAll: true,                   // 과거 빌드의 HTML 리포트도 히스토리로 다 남김
                reportDir: 'build/reports',      // HTML 파일이 생성되는 폴더 경로
                reportFiles: 'Test_Report.html', // gtest_to_html.ps1에서 생성한 메인 HTML 파일
                reportName: 'GoogleTest Report'  // 젠킨스 좌측 메뉴에 표시될 탭 이름
            ])



            // xUnit 플러그인을 호출하여 구글테스트 결과 XML 파싱
            xunit tools: [GoogleTest(
                pattern: 'build/reports/**/*.xml', // 결과 파일 경로
                deleteOutputFiles: true,             // 변환 후 임시 파일 삭제 여부
                skipNoTestFiles: false
            )]
        }
    }
}
