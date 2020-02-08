FROM python:3.6
RUN pip install flask mongoengine bcrypt
COPY *.py ./
EXPOSE 80
CMD flask run --host 0.0.0.0 --port 80
