# 💻 Deploy to VPS (DigitalOcean/Linode/etc.)

Complete guide for deploying to a Virtual Private Server.

## Prerequisites

- VPS with Ubuntu 22.04 LTS
- SSH access
- Domain name (optional but recommended)

## Step-by-Step Guide

### 1. Initial Server Setup

```bash
# Update system
sudo apt update && sudo apt upgrade -y

# Install essential tools
sudo apt install -y curl wget git build-essential

# Install Docker (if using Docker)
curl -fsSL https://get.docker.com -o get-docker.sh
sudo sh get-docker.sh
sudo usermod -aG docker $USER
```

### 2. Install Dependencies

#### Option A: Native Installation

```bash
# Install C++ build tools
sudo apt install -y g++ make libsqlite3-dev

# Install Python
sudo apt install -y python3 python3-pip python3-venv

# Install Node.js (for frontend build)
curl -fsSL https://deb.nodesource.com/setup_20.x | sudo -E bash -
sudo apt install -y nodejs

# Install Nginx
sudo apt install -y nginx
```

#### Option B: Docker (Recommended)

```bash
# Docker already installed from step 1
# No additional dependencies needed
```

### 3. Clone Repository

```bash
cd /opt
sudo git clone https://github.com/yourusername/Assembly-Line-Simulator.git
sudo chown -R $USER:$USER Assembly-Line-Simulator
cd Assembly-Line-Simulator
```

### 4. Build Application

#### Using Docker:

```bash
# Build image
docker build -t assembly-line:latest .

# Run container
docker run -d \
  --name assembly-line \
  -p 8000:8000 \
  -v $(pwd)/database:/app/database \
  -v $(pwd)/data:/app/data \
  --restart unless-stopped \
  assembly-line:latest
```

#### Native Build:

```bash
# Build C++ executable
make release

# Setup Python API
cd api
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt

# Build frontend
cd ../frontend
npm install
npm run build
```

### 5. Setup Process Manager (Native)

Install PM2:

```bash
sudo npm install -g pm2
```

Create ecosystem file (`ecosystem.config.js`):

```javascript
module.exports = {
  apps: [
    {
      name: 'assembly-line-api',
      script: 'api/main.py',
      interpreter: '/opt/Assembly-Line-Simulator/api/venv/bin/python',
      cwd: '/opt/Assembly-Line-Simulator',
      env: {
        PORT: 8000,
        DATABASE_PATH: '/opt/Assembly-Line-Simulator/database/assembly_line.db'
      },
      error_file: '/var/log/assembly-line-api-error.log',
      out_file: '/var/log/assembly-line-api-out.log',
      log_date_format: 'YYYY-MM-DD HH:mm:ss Z',
      merge_logs: true,
      autorestart: true,
      watch: false
    },
    {
      name: 'assembly-line-frontend',
      script: 'serve',
      args: '-s frontend/dist -l 3000',
      cwd: '/opt/Assembly-Line-Simulator',
      env: {
        NODE_ENV: 'production'
      },
      error_file: '/var/log/assembly-line-frontend-error.log',
      out_file: '/var/log/assembly-line-frontend-out.log',
      autorestart: true
    }
  ]
};
```

Start services:

```bash
cd /opt/Assembly-Line-Simulator
pm2 start ecosystem.config.js
pm2 save
pm2 startup  # Run the command it outputs
```

### 6. Configure Nginx

Create `/etc/nginx/sites-available/assembly-line`:

```nginx
server {
    listen 80;
    server_name yourdomain.com www.yourdomain.com;

    # Frontend
    location / {
        proxy_pass http://localhost:3000;
        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection 'upgrade';
        proxy_set_header Host $host;
        proxy_cache_bypass $http_upgrade;
    }

    # API
    location /api {
        rewrite ^/api/(.*) /$1 break;
        proxy_pass http://localhost:8000;
        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection 'upgrade';
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
        proxy_cache_bypass $http_upgrade;
    }

    # WebSocket support
    location /ws {
        proxy_pass http://localhost:8000;
        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection "upgrade";
        proxy_set_header Host $host;
        proxy_read_timeout 86400;
    }
}
```

Enable site:

```bash
sudo ln -s /etc/nginx/sites-available/assembly-line /etc/nginx/sites-enabled/
sudo nginx -t
sudo systemctl reload nginx
```

### 7. Setup SSL with Let's Encrypt

```bash
sudo apt install certbot python3-certbot-nginx
sudo certbot --nginx -d yourdomain.com -d www.yourdomain.com
```

Certbot will auto-renew certificates.

### 8. Firewall Configuration

```bash
sudo ufw allow 22/tcp
sudo ufw allow 80/tcp
sudo ufw allow 443/tcp
sudo ufw enable
```

### 9. Monitoring

Check logs:

```bash
# PM2 logs
pm2 logs

# Nginx logs
sudo tail -f /var/log/nginx/access.log
sudo tail -f /var/log/nginx/error.log

# System logs
journalctl -u nginx -f
```

### 10. Auto-update Script (Optional)

Create `/opt/update-assembly-line.sh`:

```bash
#!/bin/bash
cd /opt/Assembly-Line-Simulator
git pull origin main
make release
cd api
source venv/bin/activate
pip install -r requirements.txt --upgrade
cd ../frontend
npm install
npm run build
pm2 restart all
```

Make executable:

```bash
chmod +x /opt/update-assembly-line.sh
```

## Cost Comparison

| Provider | Cost | RAM | Storage | Best For |
|----------|------|-----|---------|----------|
| DigitalOcean | $6/mo | 1GB | 25GB | General use |
| Linode | $5/mo | 1GB | 25GB | Budget |
| Hetzner | €4/mo | 2GB | 20GB | Europe |
| AWS EC2 | $5-15/mo | 1GB | 8GB | AWS ecosystem |

## Security Checklist

- ✅ Firewall configured (UFW)
- ✅ SSH key authentication only
- ✅ SSL certificate installed
- ✅ Regular system updates
- ✅ Strong passwords/service keys
- ✅ Database backups
- ✅ Fail2ban (optional)

## Backup Strategy

```bash
# Backup database
tar -czf backup-$(date +%Y%m%d).tar.gz database/

# Setup cron for daily backups
crontab -e
# Add: 0 2 * * * tar -czf /backups/assembly-line-$(date +\%Y\%m\%d).tar.gz /opt/Assembly-Line-Simulator/database/
```

## Troubleshooting

**Service not starting?**
- Check PM2 logs: `pm2 logs assembly-line-api`
- Check system resources: `htop`
- Verify ports: `sudo netstat -tlnp`

**Nginx errors?**
- Test config: `sudo nginx -t`
- Check error log: `sudo tail -f /var/log/nginx/error.log`

**Database issues?**
- Check permissions: `ls -la database/`
- Verify path in environment variables

