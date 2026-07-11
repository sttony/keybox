### Deploy the sample application

The Serverless Application Model Command Line Interface (SAM CLI) is an extension of the AWS CLI that adds functionality for building and testing Lambda applications. It uses Docker to run your functions in an Amazon Linux environment that matches Lambda. It can also emulate your application's build environment and API.

To use the SAM CLI, you need the following tools.

- SAM CLI - [Install the SAM CLI](https://docs.aws.amazon.com/serverless-application-model/latest/developerguide/serverless-sam-cli-install.html)
- [Python 3 installed](https://www.python.org/downloads/)
- Docker - [Install Docker community edition](https://hub.docker.com/search/?type=edition&offering=community)

To build and deploy your application for the first time, run the following in your shell:

```bash
sam build --use-container
sam deploy --guided
```

The first command will build the source of your application. The second command will package and deploy your application to AWS, with a series of prompts:

- **Stack Name**: The name of the stack to deploy to CloudFormation. This should be unique to your account and region, and a good starting point would be something matching your project name.
- **AWS Region**: The AWS region you want to deploy your app to.
- **Confirm changes before deploy**: If set to yes, any change sets will be shown to you before execution for manual review. If set to no, the AWS SAM CLI will automatically deploy application changes.
- **Allow SAM CLI IAM role creation**: Many AWS SAM templates, including this example, create AWS IAM roles required for the AWS Lambda function(s) included to access AWS services. By default, these are scoped down to minimum required permissions. To deploy an AWS CloudFormation stack which creates or modifies IAM roles, the `CAPABILITY_IAM` value for `capabilities` must be provided. If permission isn't provided through this prompt, to deploy this example you must explicitly pass `--capabilities CAPABILITY_IAM` to the `sam deploy` command.
- **Save arguments to samconfig.toml**: If set to yes, your choices will be saved to a configuration file inside the project, so that in the future you can just re-run `sam deploy` without parameters to deploy changes to your application.

### Multi-stage Deployment

You can deploy the application to different stages (e.g., beta, prod) by using the `STAGE` parameter.

#### Deploy Beta

```bash
scripts/deploy_beta.sh
```

#### Deploy Product

```bash
scripts/deploy_prod.sh
```

Both scripts deploy from this local repository checkout. The SAM/Lambda stack is built from `keybox_web/`, and static content is uploaded from `static_content/`. The beta script uses stack `keyboxw-beta`, Lambda stage `beta`, Lambda function `keyboxw-beta-keybox`, and static bucket `beta-keybox-static`. The product script uses stack `keyboxw-prod`, Lambda stage `prod`, Lambda function `keyboxw-prod-keybox`, and static bucket `prod-keybox-static`. Static buckets are created automatically when they do not exist.

After deploy, the script validates `https://api.k3ybox.us/hello` and `https://www.k3ybox.us/privacy` with HTTPS requests. Use `--validation-url` or `--static-validation-url` to check different endpoints, or `--skip-validation` to disable the checks.

Use `scripts/deploy.sh` directly when you need overrides:

```bash
scripts/deploy.sh --stage prod --static-bucket prod-keybox-static --distribution-id E1234567890
```

Create or reuse a CloudFront distribution for the static site with:

```bash
scripts/deploy.sh --stage prod --ensure-cloudfront --certificate-arn arn:aws:acm:us-east-1:123456789012:certificate/example
```

The certificate must be an ACM certificate in `us-east-1` that covers `www.k3ybox.us`. The script configures the static S3 bucket for private CloudFront access with Origin Access Control, creates a CloudFront Function for extensionless paths such as `/privacy`, and prints the CloudFront domain name to use for Route 53 `A` and `AAAA` alias records.

You can find your API Gateway Endpoint URL in the output values displayed after deployment.

### Use the SAM CLI to build and test locally

Build your application with the `sam build --use-container` command.

```bash
keyboxw$ sam build --use-container
```

The SAM CLI installs dependencies defined in `hello_world/requirements.txt`, creates a deployment package, and saves it in the `.aws-sam/build` folder.

Test a single function by invoking it directly with a test event. An event is a JSON document that represents the input that the function receives from the event source. Test events are included in the `events` folder in this project.

Run functions locally and invoke them with the `sam local invoke` command.

```bash
keyboxw$ sam local invoke HelloWorldFunction --event events/event.json
```

The SAM CLI can also emulate your application's API. Use the `sam local start-api` to run the API locally on port 3000.

```bash
keyboxw$ sam local start-api
keyboxw$ curl http://localhost:3000/
```

The SAM CLI reads the application template to determine the API's routes and the functions that they invoke. The `Events` property on each function's definition includes the route and method for each path.

```yaml
Events:
  HelloWorld:
    Type: Api
    Properties:
      Path: /hello
      Method: get
```

### Add a resource to your application

The application template uses AWS Serverless Application Model (AWS SAM) to define application resources. AWS SAM is an extension of AWS CloudFormation with a simpler syntax for configuring common serverless application resources such as functions, triggers, and APIs. For resources not included in [the SAM specification](https://github.com/awslabs/serverless-application-model/blob/master/versions/2016-10-31.md), you can use standard [AWS CloudFormation](https://docs.aws.amazon.com/AWSCloudFormation/latest/UserGuide/aws-template-resource-type-ref.html) resource types.

### Fetch, tail, and filter Lambda function logs

To simplify troubleshooting, SAM CLI has a command called `sam logs`. `sam logs` lets you fetch logs generated by your deployed Lambda function from the command line. In addition to printing the logs on the terminal, this command has several nifty features to help you quickly find the bug.

`NOTE`: This command works for all AWS Lambda functions; not just the ones you deploy using SAM.

```bash
keyboxw$ sam logs -n HelloWorldFunction --stack-name keyboxw --tail
```

You can find more information and examples about filtering Lambda function logs in the [SAM CLI Documentation](https://docs.aws.amazon.com/serverless-application-model/latest/developerguide/serverless-sam-cli-logging.html).

### Tests

Tests are defined in the `tests` folder in this project. Use PIP to install the test dependencies and run tests.

```bash
keyboxw$ pip install -r tests/requirements.txt --user
# unit test
keyboxw$ python -m pytest tests/unit -v
# integration test, requiring deploying the stack first.
# Create the env variable AWS_SAM_STACK_NAME with the name of the stack we are testing
keyboxw$ AWS_SAM_STACK_NAME="keyboxw" python -m pytest tests/integration -v
```

### Cleanup

To delete the sample application that you created, use the AWS CLI. Assuming you used your project name for the stack name, you can run the following:

```bash
sam delete --stack-name "keyboxw"
```

## Static Content Deployment Script

Static web assets for direct S3 hosting are in `static_content/`. Deploy only the static content with:

```bash
cd keyboxw
scripts/deploy_static_site.sh --bucket beta-keybox-static --region us-west-2 --profile default
```

The script creates the bucket when it does not exist. It precompresses HTML, CSS, JavaScript, JSON, SVG, and text files with gzip, sets conservative cache headers for HTML, sets long cache headers for versioned assets, and can invalidate CloudFront when `--distribution-id` is provided.

## Resources

See the [AWS SAM developer guide](https://docs.aws.amazon.com/serverless-application-model/latest/developerguide/what-is-sam.html) for an introduction to SAM specification, the SAM CLI, and serverless application concepts.

Next, you can use AWS Serverless Application Repository to deploy ready to use Apps that go beyond hello world samples and learn how authors developed their applications: [AWS Serverless Application Repository main page](https://aws.amazon.com/serverless/serverlessrepo/)
