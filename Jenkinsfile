pipeline {
    // 사용 가능한 Jenkins 에이전트에서 실행한다.
    agent any

    stages {
        // CMake 구성 단계: 빌드 시스템 파일을 생성한다.
        stage('Configure') {
            steps {
                script {
                    // OS에 따라 셸 명령(sh/bat)을 분기한다.
                    if (isUnix()) {
                        sh 'cmake -S . -B build -DCMAKE_BUILD_TYPE=Release'
                    } else {
                        bat 'cmake -S . -B build -DCMAKE_BUILD_TYPE=Release'
                    }
                }
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
    }

    post {
        always {
            // 테스트 실패 여부와 무관하게 XML 리포트를 수집해 Jenkins 테스트 리포트로 표시한다.
            junit allowEmptyResults: true, testResults: 'build/reports/*.xml'
            // 사후 분석을 위해 XML 리포트를 아티팩트로 항상 보관한다.
            archiveArtifacts artifacts: 'build/reports/*.xml', onlyIfSuccessful: false
        }
    }
}
