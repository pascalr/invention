#!/bin/bash
(cd client && RAILS_ENV=production rails assets:precompile)
(cd client && RAILS_SERVE_STATIC_FILES=1 rails server -p 3001 -e production)
