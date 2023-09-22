# opGUI CI/CD guide

## Options
1. **Running Jenkins inside a docker container (for Debian/MacOs)**

 - Install Docker from [here](https://docs.docker.com/engine/install/).
 - Create Docker Image for running jenkins
    ```bash
        cd <path_to_opgui>/utils/ci/jenkins
        DOCKER_BUILDKIT=1 docker build -f utils/ci/jenkins/Dockerfile -t custom-jenkins-image .
     ```
- Run Docker Container
    ```bash
       docker run -u 0 --privileged --name jenkins2 -it -d -p 8080:8080 -p 50000:50000 -v /var/run/docker.sock:/var/run/docker.sock -v $(pwd)/data:/var/jenkins_home custom-jenkins-image
    ```
- Create Docker Image for running build and test for opGUI
    ```bash
       cd <path_to_opgui>/utils/ci/
       DOCKER_BUILDKIT=1 docker build -f utils/ci/Dockerfile -t ci-cd-unfallvorschung:latest .
    ```
- Configure Jenkins user data
    Open Browser and navigate to localhost:8080. Add the password that Jenkins requires and set username and pwd for Jenkins. 

- Install Jenkins plugins
    Inside the Jenkins UI navigate to "Manage Jenkins" and select "Plugins". Install "Docker Pipeline" and "Docker plugin"

- Create Pipeline for Build and Test
    Click on "create new item" and select pipe. Give a name to the pipe, click on configure and add the contents of the file <path_to_opgui>/utils/ci/jenkins/JenkinsFile into the Pipeline Script text input field.
