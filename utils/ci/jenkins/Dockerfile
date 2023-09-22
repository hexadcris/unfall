FROM jenkins/jenkins:latest

USER root

# Install Docker and other dependencies
RUN apt-get update && \
    apt-get install -y docker.io && \
    apt-get clean

# Add jenkins user to the docker group
RUN usermod -aG docker jenkins

USER jenkins


#DOCKER_BUILDKIT=1 docker build -f utils/ci/jenkins/Dockerfile -t custom-jenkins-image .
#docker run -u 0 --privileged --name jenkins2 -it -d -p 8080:8080 -p 50000:50000 -v /var/run/docker.sock:/var/run/docker.sock -v $(pwd)/data:/var/jenkins_home custom-jenkins-image