pipeline {
    agent any

    stages {
        stage('Configure') {
            steps {
                script {
                    if (isUnix()) {
                        sh 'cmake -S . -B build -DCMAKE_BUILD_TYPE=Release'
                    } else {
                        bat 'cmake -S . -B build -DCMAKE_BUILD_TYPE=Release'
                    }
                }
            }
        }

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
            junit allowEmptyResults: true, testResults: 'build/reports/*.xml'
            archiveArtifacts artifacts: 'build/reports/*.xml', onlyIfSuccessful: false
        }
    }
}
