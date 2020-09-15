# RUN FROM ROOT DIRECTORY ex. ./scripts/<script>.sh
# First time set up for Docker & Docker Compose
curl -fsSL https://get.docker.com -o get-docker.sh
sudo sh get-docker.sh
sudo usermod -aG docker $USER
sudo apt-get install -y docker-compose