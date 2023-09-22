FROM node:18-bullseye-slim AS deps-image 
WORKDIR /root/

RUN npm install -g selenium-webdriver 
RUN npm install -g mocha
RUN apt-get update
RUN apt-get install -y git make cmake gcc g++ qtbase5-dev python3 wget gpg python3-sphinx python3-sphinx-rtd-theme python3-sphinx-tabs python3-sphinx-tabs-doc googletest libgtest-dev

## INSTALL CHROME DRIVERS FOR end to end testing
RUN apt install -y fonts-liberation libasound2 libnspr4 libnss3 libu2f-udev xdg-utils curl unzip

FROM deps-image AS install-image
## INSTALL CHROME on the system l

RUN mkdir -p ~/bin/chrome
RUN wget -q -O ~/chrome-linux.zip https://edgedl.me.gvt1.com/edgedl/chrome/chrome-for-testing/116.0.5845.96/linux64/chrome-linux64.zip && unzip -q ~/chrome-linux.zip -d ~/bin/chrome

##INSTALL CHROME DRIVER

RUN wget -q -O ~/chromedriver.zip https://edgedl.me.gvt1.com/edgedl/chrome/chrome-for-testing/116.0.5845.96/linux64/chromedriver-linux64.zip && unzip -q ~/chromedriver.zip -d ~/bin/chrome
RUN chmod +x ~/bin/chrome/chromedriver-linux64/chromedriver 

# RUN rm ~/chromedriver.zip && ~/chrome-linux.zip
RUN echo 'export PATH="$PATH:$HOME/bin:~/bin/chrome/chrome-linux64:~/bin/chrome/chromedriver-linux64/"' >> ~/.bashrc
# RUN source ~/.bashrc
#create folder structure for backend tests
RUN mkdir -p ~/opgui ~/opgui-user/opSimulation/bin/core ~/opgui-user/opSimulation/bin/core/components ~/opgui-user/opSimulation/bin/core/modules ~/opgui-user/workspace1/systems && \
    touch ~/opgui-user/opSimulation/bin/core/opSimulation ~/opgui-user/opSimulation/bin/core/opSimulationManager


#DOCKER_BUILDKIT=1 docker build -f utils/ci/Dockerfile -t ci-cd-unfallvorschung:latest . 