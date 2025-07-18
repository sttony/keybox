from aws_lambda_powertools.event_handler import APIGatewayRestResolver
from aws_lambda_powertools.utilities.typing import LambdaContext
from aws_lambda_powertools.logging import correlation_paths
from aws_lambda_powertools import Logger
from aws_lambda_powertools import Tracer
from aws_lambda_powertools import Metrics
from aws_lambda_powertools.metrics import MetricUnit

from refresh_token import handler as refresh_handler
from register import handler as register_handler
from activate import handler as activate_handler
from retrieve import handler as retrieve_handler
from upload import handler as upload_handler
from setup_new_client import handler as setup_new_client_handler

app = APIGatewayRestResolver()
tracer = Tracer()
logger = Logger()
metrics = Metrics(namespace="Powertools")


@app.get("/hello")
@tracer.capture_method
def hello():
    # adding custom metrics
    # See: https://awslabs.github.io/aws-lambda-powertools-python/latest/core/metrics/
    metrics.add_metric(name="HelloWorldInvocations", unit=MetricUnit.Count, value=1)

    # structured log
    # See: https://awslabs.github.io/aws-lambda-powertools-python/latest/core/logger/
    logger.info("Hello world API - HTTP 200")
    return {"message": "hello world"}, 404


@app.get("/refresh_token")
@tracer.capture_method
def refresh_token():
    return refresh_handler.lambda_handler(app.current_event, app.context)


@app.post("/register")
@tracer.capture_method
def register():
    return register_handler.lambda_handler(app.current_event, app.context)


@app.get("/activate")
@tracer.capture_method
def activate():
    return activate_handler.lambda_handler(app.current_event, app.context)

@app.post('/retrieve')
@tracer.capture_method
def retrieve():
    return retrieve_handler.lambda_handler(app.current_event, app.context)

@app.post('/upload')
@tracer.capture_method
def upload():
    return upload_handler.lambda_handler(app.current_event, app.context)

@app.post('/setup_new_client')
@tracer.capture_method
def upload():
    return setup_new_client_handler.lambda_handler(app.current_event, app.context)

# Enrich logging with contextual information from Lambda
@logger.inject_lambda_context(correlation_id_path=correlation_paths.API_GATEWAY_REST)
# Adding tracer
# See: https://awslabs.github.io/aws-lambda-powertools-python/latest/core/tracer/
@tracer.capture_lambda_handler
# ensures metrics are flushed upon request completion/failure and capturing ColdStart metric
@metrics.log_metrics(capture_cold_start_metric=True)
def lambda_handler(event: dict, context: LambdaContext) -> dict:
    return app.resolve(event, context)
